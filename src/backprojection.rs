use crate::bresenham::bresenham_line;
use std::f32::consts::PI;

/// Reconstructs an image from a sinogram using simple back-projection.
///
/// * `sinogram` – Vec of projection rows (one per angle), as produced by `radon::generate_sinogram`
/// * `size`     – output image side length in pixels
/// * `step`     – angular step in degrees (same value used when generating the sinogram)
///
/// Returns a `size × size` float image.
pub fn backproject(sinogram: Vec<Vec<f32>>, size: usize, step: f32) -> Vec<Vec<f32>> {
    let mut image = vec![vec![0.0f32; size]; size];
    if sinogram.is_empty() || size == 0 {
        return image;
    }

    let detectors = sinogram[0].len();
    let center = (size as f32 - 1.0) / 2.0;
    let radius = center;

    for (i, row) in sinogram.iter().enumerate() {
        let angle_deg = i as f32 * step;
        let angle_rad = angle_deg * PI / 180.0;
        let cos_a = angle_rad.cos();
        let sin_a = angle_rad.sin();

        for d in 0..detectors {
            let t = if detectors > 1 {
                -radius + (2.0 * radius) * (d as f32) / (detectors as f32 - 1.0)
            } else {
                0.0
            };

            let x0 = (center + t * (-sin_a) - radius * cos_a).round() as i32;
            let y0 = (center + t * cos_a  - radius * sin_a).round() as i32;
            let x1 = (center + t * (-sin_a) + radius * cos_a).round() as i32;
            let y1 = (center + t * cos_a  + radius * sin_a).round() as i32;

            let value = row[d];
            for (x, y) in bresenham_line(x0, y0, x1, y1) {
                if x >= 0 && y >= 0 && (x as usize) < size && (y as usize) < size {
                    image[x as usize][y as usize] += value;
                }
            }
        }
    }
    image
}
