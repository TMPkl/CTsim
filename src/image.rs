use crate::pixel::Pixel;
use crate::point::Point;

#[derive(Debug, Clone)]
pub struct Image {
    width: usize,
    height: usize,
    pixels: Vec<Vec<Pixel>>,
}

impl Image {
    pub fn new(width: usize, height: usize) -> Self {
        let pixels = vec![vec![Pixel::new(0, Point::new(0, 0)); width]; height];
        Self {
            width,
            height,
            pixels,
        }
    }
}