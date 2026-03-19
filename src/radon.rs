pub mod pixel;
pub mod point;
pub mod image;
pub mod lib;
pub fn radon_transform(image: &PyImage, angle_iter_range: (i8, i8), angle_step: f64, n_emitors: i64, ang_detect_range: f64) -> &PyImage {
    // Implementation for Radon transform
}
