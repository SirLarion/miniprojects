use phf::phf_map;

pub const FINGRID_ENV_PATH: &'static str = "/home/sirlarion/repos/miniprojects/fingrid-data/";
pub const FINGRID_DATA_URL: &'static str =
    "https://www.fingrid.fi/api/graph/power-system-state?language=fi";

pub const TABLE_QUERY: &'static str = "
    CREATE TABLE IF NOT EXISTS fingrid_data (
        id INTEGER PRIMARY KEY,
        production REAL NOT NULL,
        consumption REAL NOT NULL,
        net_import_export REAL NOT NULL,
        hydro_power REAL NOT NULL,
        nuclear_power REAL NOT NULL,
        cogen_industry REAL NOT NULL,
        cogen_heating REAL NOT NULL,
        wind_power REAL NOT NULL,
        solar_power REAL NOT NULL,
        other_production REAL NOT NULL,
        price REAL NOT NULL,
        import_se REAL NOT NULL,
        import_ee REAL NOT NULL,
        import_no REAL NOT NULL,
        timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
    );
";

pub const INSERT_DATA_QUERY: &'static str = "
    INSERT INTO fingrid_data (
        production, 
        consumption,
        net_import_export,
        hydro_power,
        nuclear_power,
        cogen_industry,
        cogen_heating,
        wind_power,
        solar_power,
        other_production,
        price,
        import_se,
        import_ee,
        import_no
    ) VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14);
";

pub const FINGRID_BASE_URL: &'static str = "https://data.fingrid.fi/api/datasets/";

pub const API_IDS: phf::Map<&'static str, u32> = phf_map! {
    "total" => 192,
    "consumption" => 193,
    "hydro" => 191,
    "wind" => 181,
    "nuclear" => 188,
    "solar" => 248,
    "district_heating" => 201,
    "industrial_heating" => 202,
    "se1" => 87,
    "se_aland" => 89,
    "se3" => 90,
    "estonia" => 180,
    "norway" => 187,
    "price" => 317
};
