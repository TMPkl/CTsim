use crate::pixel::Pixel;
use crate::point::Point;
use crate::PyImage;

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

    pub fn new_from_py_image(py_img: &PyImage) -> Self {
        let width = py_img.width_rust();
        let height = py_img.height_rust();
        let pixels = (0..height)
            .map(|y| {
                (0..width)
                    .map(|x| Pixel::new(py_img.get_pixel_rust(x, y), Point::new(x as i32, y as i32)))
                    .collect()
            })
            .collect();

        Self {
            width,
            height,
            pixels,
        }
    }
    pub fn get_pixel_value(&self, x: usize, y: usize)-> u8 {
        return self.pixels[y][x].get();
    }
    pub fn set_pixel_value(&mut self, point: &Point, value: u8){
        self.pixels[point.y as usize][point.x as usize].set(value);
    }

    pub fn in_bounds(&self, x: usize, y: usize) -> bool{
        return x< self.width && y<self.height
    }

    pub fn width(&self) -> usize {
        self.width
    }

    pub fn height(&self) -> usize {
        self.height
    }

    pub fn to_vec2_u8(&self) -> Vec<Vec<u8>> {
        self
            .pixels
            .iter()
            .map(|row| row.iter().map(Pixel::get).collect())
            .collect()
    }
    
    pub fn draw_line(&mut self, start: &Point, end: &Point, value: u8){
        let dx = (end.x - start.x).abs();
        let dy = (end.y - start.y).abs();
        let sx = if start.x < end.x { 1 } else { -1 };
        let sy = if start.y < end.y { 1 } else { -1 };
        let mut err = dx - dy;

        let mut x = start.x;
        let mut y = start.y;

        loop {
            if self.in_bounds(x as usize, y as usize) {
                self.set_pixel_value(&Point::new(x, y), value);
            }
            if x == end.x && y == end.y {
                break;
            }
            let err2 = err * 2;
            if err2 > -dy {
                err -= dy;
                x += sx;
            }
            if err2 < dx {
                err += dx;
                y += sy;
            }
        }
    }

    pub fn sum_pixel_bresenham(&mut self, start: &Point, angle_deg: f64) -> f64 {
        let angle_rad = angle_deg.to_radians();
        let end_x = start.x + (self.width as f64 * angle_rad.cos()) as i32;
        let end_y = start.y + (self.height as f64 * angle_rad.sin()) as i32;
        let end_point = Point::new(end_x, end_y);
        let mut sum = 0.0f64;

        let dx = (end_point.x - start.x).abs();
        let dy = (end_point.y - start.y).abs();
        let sx = if start.x < end_point.x { 1 } else { -1 };
        let sy = if start.y < end_point.y { 1 } else { -1 };
        let mut err = dx - dy;

        let mut x = start.x;
        let mut y = start.y;

        loop {
            if self.in_bounds(x as usize, y as usize) {
                sum += self.get_pixel_value(x as usize, y as usize) as f64;
            }
            if x == end_point.x && y == end_point.y {
                break;
            }
            let err2 = err * 2;
            if err2 > -dy {
                err -= dy;
                x += sx;
            }
            if err2 < dx {
                err += dx;
                y += sy;
            }
        }

        return sum;
    }

}