pub mod radon;
pub mod bresenham;
pub mod backprojection;

use pyo3::prelude::*;

#[pyfunction]
fn generate_sinogram_py(image: Vec<Vec<f32>>, detectors: usize, step: f32) -> Vec<Vec<f32>> {
    crate::radon::generate_sinogram(image, detectors, step)
}

#[pymodule]
fn tomography(m: &Bound<'_, PyModule>) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(generate_sinogram_py, m)?)?;
    Ok(())
}