use crate::point::Point;

#[derive(Debug, Clone)]
pub struct Pixel {
    value: u8,
    position: Point,
}

impl Pixel {
    pub fn new(value: u8, position: Point) -> Self {
        Self {
            value,
            position,
        }
    }
}