use std::{
    env,
    io::{self, ErrorKind},
};

use dotenv::dotenv;
use reqwest as req;
use reqwest::header::{HeaderMap, HeaderValue, CONTENT_TYPE};
use serde::Deserialize;

pub mod constants;
pub mod error;

use constants::*;
use error::*;
use sqlx::{Sqlite, SqlitePool};

#[derive(Deserialize, Clone)]
struct PowerTransferMapValue {
    #[serde(rename = "Key")]
    id: String,
    #[serde(rename = "Value")]
    amount: Option<f32>,
}

#[derive(Deserialize)]
struct ApiData {
    #[serde(rename = "Consumption")]
    consumption: f32,
    #[serde(rename = "Production")]
    production: f32,
    #[serde(rename = "NetImportExport")]
    net_import_export: f32,
    #[serde(rename = "HydroPower")]
    hydro_power: f32,
    #[serde(rename = "NuclearPower")]
    nuclear_power: f32,
    #[serde(rename = "CogenerationIndustry")]
    cogen_industry: f32,
    #[serde(rename = "CogenerationDistrictHeating")]
    cogen_heating: f32,
    #[serde(rename = "WindPower")]
    wind_power: f32,
    #[serde(rename = "SolarPower")]
    solar_power: f32,
    #[serde(rename = "OtherProduction")]
    other_production: f32,
    #[serde(rename = "ElectricityPriceInFinland")]
    price: f32,
    #[serde(rename = "PowerTransferMap")]
    transfer_map: [PowerTransferMapValue; 6],
}

fn load_env() -> Result<(), AppError> {
    env::set_current_dir(FINGRID_ENV_PATH).map_err(|_| {
        AppError::IoError(io::Error::new(
            ErrorKind::NotFound,
            "findgrid-data directory was not found",
        ))
    })?;

    dotenv().ok();
    Ok(())
}

fn get_headers() -> Result<HeaderMap, AppError> {
    let key = env::var("X_API_KEY")?;

    let mut headers = HeaderMap::new();
    headers.insert("x-api-key", HeaderValue::from_str(key.as_str())?);
    headers.insert(CONTENT_TYPE, HeaderValue::from_static("application/json"));

    Ok(headers)
}

#[tokio::main]
async fn main() -> Result<(), AppError> {
    load_env()?;
    let pool = SqlitePool::connect("db.sqlite").await?;
    sqlx::query(TABLE_QUERY).execute(&pool).await?;

    let client = req::Client::new();
    let headers = get_headers()?;

    let res = client
        .get(FINGRID_DATA_URL)
        .headers(headers)
        .send()
        .await?
        .text()
        .await?;

    println!("{:?}", res);

    let data = serde_json::from_str::<ApiData>(res.as_str())?;

    let import_se = data
        .transfer_map
        .clone()
        .into_iter()
        .filter(|o| ["SEAland", "SE1", "SE3"].contains(&o.id.as_str()))
        .map(|o| o.amount)
        .sum::<Option<f32>>()
        .unwrap_or(0.0);

    let import_ee = data
        .transfer_map
        .clone()
        .into_iter()
        .find(|o| o.id == "Estonia")
        .and_then(|o| o.amount)
        .unwrap_or(0.0);

    let import_no = data
        .transfer_map
        .clone()
        .into_iter()
        .find(|o| o.id == "Norway")
        .and_then(|o| o.amount)
        .unwrap_or(0.0);

    sqlx::query::<Sqlite>(INSERT_DATA_QUERY)
        .bind(data.production)
        .bind(data.consumption)
        .bind(data.net_import_export)
        .bind(data.hydro_power)
        .bind(data.nuclear_power)
        .bind(data.cogen_industry)
        .bind(data.cogen_heating)
        .bind(data.wind_power)
        .bind(data.solar_power)
        .bind(data.other_production)
        .bind(data.price)
        .bind(import_se)
        .bind(import_ee)
        .bind(import_no)
        .execute(&pool)
        .await?;

    Ok(())
}
