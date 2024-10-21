#include "glad/gl.h"
#include <cassert>

#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "GlCall.h"
#include "Renderer.hpp"

VertexArray::VertexArray() {
    GLCALL(glGenVertexArrays(1, &m_RenderID));
}
VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_RenderID);
}
void VertexArray::bind() const {
    GLCALL(glBindVertexArray(m_RenderID));
}
void VertexArray::unbind() const {
    GLCALL(glBindVertexArray(0));
}

void VertexArray::addBuffer(VertexBuffer const &VB, VertexBufferlayout const &layout) {
    bind();
    VB.bind();
    auto const& elements = layout.getElements(); 
    unsigned offset = 0;
    for(unsigned i = 0; i < elements.size(); ++i) {
        auto const& element = elements.at(i);
        GLCALL(glVertexAttribPointer(i, element.count, element.type, element.normalised, layout.getStride(), reinterpret_cast<void const *>(offset)));
        GLCALL(glEnableVertexAttribArray(i));
        offset += element.count * getSizeOfGLType(element.type);
    }
}