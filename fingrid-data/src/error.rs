use std::{io, num, env};

use thiserror::Error;

#[derive(Error, Debug)]
pub enum AppError {
  #[error(transparent)]
  IoError(#[from] io::Error),

  #[error(transparent)]
  DbError(#[from] sqlx::Error),

  #[error(transparent)]
  VarError(#[from] env::VarError),

  #[error(transparent)]
  HeaderError(#[from] reqwest::header::InvalidHeaderValue),

  #[error(transparent)]
  ParseFloatError(#[from] num::ParseFloatError),

  #[error(transparent)]
  ParseIntError(#[from] num::ParseIntError),

  #[error(transparent)]
  HttpError(#[from] reqwest::Error),

  #[error(transparent)]
  SerdeError(#[from] serde_json::Error),

  #[error("executing command failed: {0}")]
  CmdError(String),
}


