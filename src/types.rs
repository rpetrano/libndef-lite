//! Various payload types to handle the parsing and validation of each data type's payload

use std::convert::{TryFrom, Into};

pub trait Payload<T> 
where T: TryFrom<Vec<u8>> + Into<Vec<u8>> {
    /// Static creation method that converts the payload bytes to the specific struct
    fn new(bytes: Vec<u8>) -> Self;
}