pub mod point;
pub mod pixel;
pub mod image;

use crate::image::Image;
use crate::point::Point;
use numpy::PyArray2;
use pyo3::exceptions::PyIndexError;
use pyo3::exceptions::PyValueError;
use pyo3::prelude::*;

#[pyclass]
pub struct PyImage {
	inner: Image,
}

#[pymethods]
impl PyImage {
	#[new]
	fn new(width: usize, height: usize) -> Self {
		Self {
			inner: Image::new(width, height),
		}
	}

	fn width(&self) -> usize {
		self.inner.width()
	}

	fn height(&self) -> usize {
		self.inner.height()
	}

	fn set_pixel(&mut self, x: usize, y: usize, value: u8) -> PyResult<()> {
		if !self.inner.in_bounds(x, y) {
			return Err(PyIndexError::new_err("pixel coordinates are out of bounds"));
		}

		self.inner.set_pixel_value(&Point::new(x as i32, y as i32), value);
		Ok(())
	}

	fn get_pixel(&self, x: usize, y: usize) -> PyResult<u8> {
		if !self.inner.in_bounds(x, y) {
			return Err(PyIndexError::new_err("pixel coordinates are out of bounds"));
		}

		Ok(self.inner.get_pixel_value(x, y))
	}

	fn draw_line(&mut self, x0: i32, y0: i32, x1: i32, y1: i32, value: u8) {
		let start = Point::new(x0, y0);
		let end = Point::new(x1, y1);
		self.inner.draw_line(&start, &end, value);
	}

	fn to_numpy<'py>(&self, py: Python<'py>) -> PyResult<Bound<'py, PyArray2<u8>>> {
		PyArray2::from_vec2(py, &self.inner.to_vec2_u8())
			.map_err(|err| PyValueError::new_err(err.to_string()))
	}
}

#[pymodule]
fn tomography(m: &Bound<'_, PyModule>) -> PyResult<()> {
	m.add_class::<PyImage>()?;
	Ok(())
}