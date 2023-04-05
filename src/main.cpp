// Third party includes
#include "glapplication.h"
#include "glrenderpipeline.h"
#include "gltinyobjloader.h"
#include "glsimplecamera.h"

// Own includes
#include "hyperbolicmath.h"

using attr = lithium::VertexArrayBuffer::AttributeType;
static constexpr attr POSITION{attr::VEC3};
static constexpr attr NORMAL{attr::VEC3};
static constexpr attr UV{attr::VEC2};
static constexpr attr COLOR{attr::VEC3};

class BasicPipeline : public lithium::RenderPipeline
{   
public:
    BasicPipeline(const glm::ivec2& resolution) : lithium::RenderPipeline{resolution},
        _camera{new lithium::SimpleCamera(glm::perspective(glm::radians(45.0f), (float)resolution.x / (float)resolution.y, 0.1f, 100.0f))}
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(3.0f);
        _screenMesh.reset(new lithium::Mesh({POSITION, NORMAL, UV, COLOR}, {
            -1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
            -1.0,  1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	0.0f, 1.0,    0.0f, 1.0f, 0.0f,
            1.0,  1.0, 0.0f,	0.0f, 1.0f, 0.0f,	1.0, 1.0,     0.0f, 0.0f, 1.0f,
            1.0, -1.0, 0.0f, 	0.0f, 1.0f, 0.0f,	1.0, 0.0f,    1.0f, 1.0f, 0.0f
        },
        {
            0, 2, 1,
            0, 3, 2
        }));
        _blockShader = std::make_shared<lithium::ShaderProgram>("assets/blockshader.vert", "assets/blockshader.frag");
        _blockShader->setUniform("u_texture_0", 0);
        _blockShader->setUniform("u_projection", _camera->projection());
        _borderShader = std::make_shared<lithium::ShaderProgram>("assets/blockshader.vert", "assets/bordershader.frag");
        _borderShader->setUniform("u_texture_0", 0);
        _borderShader->setUniform("u_projection", _camera->projection());
        _screenShader = std::make_shared<lithium::ShaderProgram>("assets/screenshader.vert", "assets/screenshader.frag");
        _camera->setPosition(glm::vec3{3.0f, 3.0f, 3.0f});
        _camera->setTarget(glm::vec3{0.0f});
        _mainGroup = createRenderGroup([this](lithium::Renderable* renderable) -> bool {
            return dynamic_cast<lithium::Object*>(renderable);
        });
        _mainStage = addRenderStage(std::make_shared<lithium::RenderStage>(nullptr, glm::ivec4{0, 0, resolution.x, resolution.y}, [this](){
            glClearColor(0.8f, 1.0f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _blockShader->setUniform("u_view", _camera->view());
            _blockShader->setUniform("u_time", 0.0f);
            _borderShader->setUniform("u_view", _camera->view());
            _screenShader->use();
            _screenMesh->bind();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glActiveTexture(GL_TEXTURE0);
            glDepthMask(GL_FALSE);
            _screenMesh->draw();
            glDepthMask(GL_TRUE);
            _mainGroup->render(_blockShader.get());
        }));

        setViewportToResolution();
    }

    ~BasicPipeline()
    {
        _blockShader = nullptr;
        _screenShader = nullptr;
        _screenMesh = nullptr;
    }

    std::shared_ptr<lithium::SimpleCamera> camera()
    {
        return _camera;
    }

private:
    std::shared_ptr<lithium::ShaderProgram> _blockShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _borderShader{nullptr};
    std::shared_ptr<lithium::ShaderProgram> _screenShader{nullptr};
    std::shared_ptr<lithium::SimpleCamera> _camera{nullptr};
    std::shared_ptr<lithium::Object> _object;
    std::shared_ptr<lithium::Mesh> _screenMesh;

    std::shared_ptr<lithium::RenderGroup> _mainGroup;
    std::shared_ptr<lithium::RenderStage> _mainStage;
};

class BasicApp : public lithium::Application
{
public:
    BasicApp() : Application{"A simple application", glm::ivec2{1600, 900}, lithium::Application::Mode::DEFAULT, false}
    {
        _mesh = lithium::tinyobjloader_load("assets/block.obj", {POSITION, NORMAL, UV});
        _diffuseTexture.reset(lithium::ImageTexture::load("assets/Kraxbox_logo_lithium_metal_2ff2069c-b84a-426c-bf92-e9831105a5df.png", GL_SRGB, GL_RGB, GL_LINEAR, GL_REPEAT));
        _diffuseTexture->setWrap(GL_REPEAT);
        std::shared_ptr<lithium::Object> object{new lithium::Object(_mesh, {_diffuseTexture})};
        object->setPosition(glm::vec3{0.0f});
        object->setScale(0.5f);
        _renderPipeline = new BasicPipeline(defaultFrameBufferResolution());
        _renderPipeline->addRenderable(object.get());
        _objects.push_back(object);
    }

    virtual ~BasicApp() noexcept
    {
        _diffuseTexture = nullptr;
        _mesh = nullptr;
    }

    virtual void update(float dt) override
    {
        for(auto o : _objects)
        {
            o->update(dt);
            o->setRotation(o->rotation() + glm::vec3{8.0f * dt});
        }
        _renderPipeline->camera()->update(dt);
        _renderPipeline->render();
    }

    virtual void onFramebufferResized(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

private:
    BasicPipeline* _renderPipeline{nullptr};
    std::vector<std::shared_ptr<lithium::Object>> _objects;
    std::shared_ptr<lithium::Mesh> _mesh;
    std::shared_ptr<lithium::ImageTexture> _diffuseTexture;
};

constexpr double radius = 20;
constexpr double curvature = -1/(radius*radius);

int main(int argc, const char* argv[])
{
    HyperboloidGyroVector3D<double, curvature> gyroVectorTest1();
    HyperboloidGyroVector3D<double, curvature> gyroVectorTest2(1.0, 2.0, 3.0);
    HyperboloidGyroVector3D<double, curvature> gyroVectorTest3(0.0, 0.0, 0.0, 1.0);
    HyperboloidGyroVector3D<double, curvature> gyroVectorTest4(gyroVectorTest3);

    BasicApp* basicApp = new BasicApp();
    basicApp->run();
    return 0;
}