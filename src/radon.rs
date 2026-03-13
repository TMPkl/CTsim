use crate::bresenham::bresenham_line;
use std::f32::consts::PI;

/// Computes the Radon transform (sinogram) of `image`.
///
/// * `image`     – 2-D float image (row-major, square)
/// * `detectors` – number of detector elements per projection
/// * `step`      – angular step in degrees between projections
///
/// Returns a sinogram as a Vec of rows, one row per projection angle.
pub fn generate_sinogram(
    image: Vec<Vec<f32>>,
    detectors: usize,
    step: f32,
) -> Vec<Vec<f32>> {
    let n = image.len();
    if n == 0 {
        return Vec::new();
    }
    let center = (n as f32 - 1.0) / 2.0;
    let radius = center;

    let num_angles = ((180.0 / step).ceil() as usize).max(1);
    let mut sinogram = Vec::with_capacity(num_angles);

    for i in 0..num_angles {
        let angle_deg = i as f32 * step;
        let angle_rad = angle_deg * PI / 180.0;
        let cos_a = angle_rad.cos();
        let sin_a = angle_rad.sin();

        let mut row = vec![0.0f32; detectors];

        for d in 0..detectors {
            // detector position along the axis perpendicular to the ray direction
            let t = if detectors > 1 {
                -radius + (2.0 * radius) * (d as f32) / (detectors as f32 - 1.0)
            } else {
                0.0
            };

            // start and end points of the ray
            let x0 = (center + t * (-sin_a) - radius * cos_a).round() as i32;
            let y0 = (center + t * cos_a  - radius * sin_a).round() as i32;
            let x1 = (center + t * (-sin_a) + radius * cos_a).round() as i32;
            let y1 = (center + t * cos_a  + radius * sin_a).round() as i32;

            let mut sum = 0.0f32;
            for (x, y) in bresenham_line(x0, y0, x1, y1) {
                if x >= 0 && y >= 0 && (x as usize) < n && (y as usize) < image[x as usize].len() {
                    sum += image[x as usize][y as usize];
                }
            }
            row[d] = sum;
        }
        sinogram.push(row);
    }
    sinogram
}
