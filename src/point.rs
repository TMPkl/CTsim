// Example "class" template in Rust: public struct + impl block.
#[derive(Debug, Clone)]
pub struct Point {
    pub x: i32,
    pub y: i32,
}

impl Point{
    pub fn new( x: i32, y: i32) -> Self {
        Self {
            x: x,
            y: y,
        }
    }
}
