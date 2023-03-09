#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>


namespace WN = Walnut;

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render();

	std::shared_ptr<WN::Image> GetFinalImage() const { return m_FinalImage; }


private:
	uint32_t Shader(glm::vec2 coord);
private:
	std::shared_ptr<WN::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};