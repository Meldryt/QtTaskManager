#include "GlWindow.h"
#include <QTimer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLFunctions_4_5_Compatibility>
//#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>

#include <QDebug>

#ifdef _MSC_VER
#define strdup(p) _strdup(p)
#endif

static const char* m_vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 normals;\n"
"layout (location = 2) in vec2 texcoord;\n"
"layout (location = 3) in mat4 instanceMatrix;\n"
"\n"
"out vec3 fragNormals;\n"
"out vec2 fragTexCoords;\n"
"\n"
"uniform mat4 cameraMatrix;\n"
"\n"
"void main() {\n"
"   fragNormals = normals;\n"
"   fragTexCoords = texcoord;\n"
"   gl_Position = cameraMatrix * instanceMatrix * vec4(position, 1.0);\n"
"}\n";

static const char* m_fragmentShaderSource =
"#version 330 core\n"
"\n"
"in vec3 fragNormals;\n"
"in vec2 fragTexCoords;\n"
"out vec4 finalColor;\n"
"\n"
"float diffuse() {\n"
"float diff;\n"
"vec3 lightVector = vec3(1.0,1.0,0.0);\n"
"diff = max(dot(fragNormals, lightVector), 0.0);\n"
"return diff;\n"
"}\n"
"\n"
"void main() {\n"
"   if(fragTexCoords.x < 0.05 || fragTexCoords.x > 0.95 || fragTexCoords.y < 0.05 || fragTexCoords.y > 0.95)\n"
"   {\n"
"       finalColor = vec4(0,0,0,1);\n"
"   }\n"
"   else\n"
"   {\n"
"       vec3 lightColor = vec3(1.0, 0.5, 0.3);\n"
"       float ambientStrength = 0.1;\n"
"       vec3 ambientColor = ambientStrength * lightColor;\n"
"       vec3 diffuseColor = diffuse() * lightColor;\n"
"       vec3 objectColor = vec3(fragTexCoords.x, fragTexCoords.y, 0.0);\n"
"       objectColor = vec3(fragTexCoords.x, fragTexCoords.y, 0.0);\n"
"       finalColor = vec4((ambientColor + diffuseColor) * objectColor, 1.0);\n"
"   }\n"
"}\n";

namespace cube
{
    const static uint32_t vertex_comp_size{ 3 };
    const static uint32_t uv_comp_size{ 2 };

    const static std::vector<QVector3D> vertices
    {
        //front
        {-0.5, -0.5, 0.5},
        {0.5, -0.5, 0.5},
        {0.5, 0.5, 0.5},
        {-0.5, 0.5, 0.5},

        //right
        {0.5, -0.5, 0.5},
        {0.5, -0.5, -0.5},
        {0.5, 0.5, -0.5},
        {0.5, 0.5, 0.5},

        //back
        {0.5, -0.5, -0.5},
        {-0.5, -0.5, -0.5},
        {-0.5, 0.5, -0.5},
        {0.5, 0.5, -0.5},

        //left
        {-0.5, -0.5, -0.5},
        {-0.5, -0.5, 0.5},
        {-0.5, 0.5, 0.5},
        {-0.5, 0.5, -0.5},

        //top
        {-0.5, 0.5, 0.5},
        {0.5, 0.5, 0.5},
        {0.5, 0.5, -0.5},
        {-0.5, 0.5, -0.5},

        //bottom
        {0.5, -0.5, 0.5},
        {-0.5, -0.5, 0.5},
        {-0.5, -0.5, -0.5},
        {0.5, -0.5, -0.5},
    };

    const static std::vector<QVector3D> normals
    {
        //front
        {0.0, 0.0, 1.0},
        {0.0, 0.0, 1.0},
        {0.0, 0.0, 1.0},
        {0.0, 0.0, 1.0},

        //right
        {1.0, 0.0, 0.0},
        {1.0, 0.0, 0.0},
        {1.0, 0.0, 0.0},
        {1.0, 0.0, 0.0},

        //back
        {0.0,0.0,-1.0},
        {0.0,0.0,-1.0},
        {0.0,0.0,-1.0},
        {0.0,0.0,-1.0},

        //left
        {-1.0, 0.0, 0.0},
        {-1.0, 0.0, 0.0},
        {-1.0, 0.0, 0.0},
        {-1.0, 0.0, 0.0},

        //top
        {0.0, 1.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 1.0, 0.0},

        //bottom
        {0.0, -1.0, 0.0},
        {0.0, -1.0, 0.0},
        {0.0, -1.0, 0.0},
        {0.0, -1.0, 0.0},
    };

    const static std::vector<QVector2D> uvs
    {
        {0,0},
        {1,0},
        {1,1},
        {0,1},

        {0,0},
        {1,0},
        {1,1},
        {0,1},

        {0,0},
        {1,0},
        {1,1},
        {0,1},

        {0,0},
        {1,0},
        {1,1},
        {0,1},

        {0,0},
        {1,0},
        {1,1},
        {0,1},

        {0,0},
        {1,0},
        {1,1},
        {0,1},
    };

    const static std::vector<uint32_t> indices
    {
        0,1,2,
        2,3,0,

        4,5,6,
        6,7,4,

        8,9,10,
        10,11,8,

        12,13,14,
        14,15,12,

        16,17,18,
        18,19,16,

        20,21,22,
        22,23,20,
    };
}

GlWindow::GlWindow(QWidget *parent) : QOpenGLWindow()
{
    qDebug() << __FUNCTION__;

    resize(m_windowSizeX, m_windowSizeY);
    m_elapsedTimerFps = new QElapsedTimer();
    m_elapsedTimerDeltaTime = new QElapsedTimer();
    m_repaintTimer = new QTimer(this);
    //QObject::connect(m_repaintTimer, SIGNAL(timeout()), this, SLOT(update()));

    connect(this, &QOpenGLWindow::frameSwapped,
        this, QOverload<>::of(&QPaintDeviceWindow::update));
    update();
}

GlWindow::~GlWindow()
{
    qDebug() << __FUNCTION__;

    // Make sure the context is current and then explicitly
    // destroy all underlying OpenGL resources.
    //makeCurrent();
    auto itMap = m_programs.begin();
    for (;itMap != m_programs.end(); ++itMap)
    {
        delete itMap->second.program;
        delete itMap->second.texture;
        itMap->second.vbo.destroy();
        itMap->second.ibo.destroy();
        itMap->second.vao.destroy();
    }
    m_programs.clear();
    //doneCurrent();
}

bool GlWindow::createProgram(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, ProgramInfo& programInfo)
{
    bool hasTexture = false;
    char* vertexShaderSource{ nullptr };
    char* fragmentShaderSource{ nullptr };
    std::string texturePath;

    vertexShaderSource = strdup(m_vertexShaderSource);
    fragmentShaderSource = strdup(m_fragmentShaderSource);

    programInfo.indicesCount = static_cast<uint32_t>(indices.size());
    programInfo.program = new QOpenGLShaderProgram(this);
    if (!programInfo.program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource))
    {
        qDebug() << "Vertex shader errors :\n" << programInfo.program->log();
        return false;
    }
    if (!programInfo.program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource))
    {
        qDebug() << "Fragment shader errors :\n" << programInfo.program->log();
        return false;
    }
    if (!programInfo.program->link())
    {
        qDebug() << "Program linked failed :\n" << programInfo.program->log();
        return false;
    }

    programInfo.vao.create();
    if (programInfo.vao.isCreated())
    {
        programInfo.vao.bind();
    }
    else
    {
        return false;
    }

    programInfo.vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    programInfo.vbo.create();
    programInfo.vbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    programInfo.vbo.bind();
    programInfo.vbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(vertices[0])));

    programInfo.ibo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    programInfo.ibo.create();
    programInfo.ibo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    programInfo.ibo.bind();
    programInfo.ibo.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(indices[0])));

    const uint32_t vertexLocation = 0;
    const uint32_t normalLocation = 1;
    const uint32_t texcoordLocation = 2;
    const uint32_t instanceLocation = 3;

    const uint32_t vertexSize = 3;
    const uint32_t vertexOffset = 0;
    const uint32_t normalSize = 3;
    const uint32_t normalOffset = vertexOffset + vertexSize * sizeof(float);
    const uint32_t texcoordSize = 2;
    const uint32_t texcoordOffset = normalOffset + normalSize * sizeof(float);
    const uint32_t stride = texcoordOffset + texcoordSize * sizeof(float);

    programInfo.program->enableAttributeArray(vertexLocation);
    programInfo.program->setAttributeBuffer(vertexLocation, GL_FLOAT, vertexOffset, vertexSize, stride);
    programInfo.program->enableAttributeArray(normalLocation);
    programInfo.program->setAttributeBuffer(normalLocation, GL_FLOAT, normalOffset, normalSize, stride);
    programInfo.program->enableAttributeArray(texcoordLocation);
    programInfo.program->setAttributeBuffer(texcoordLocation, GL_FLOAT, texcoordOffset, texcoordSize, stride);

    m_instanceBuffer = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    m_instanceBuffer.create();
    m_instanceBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_instanceBuffer.bind();
    m_instanceBuffer.allocate(m_instancesMatrix.data(), m_instancesMatrix.size() * sizeof(m_instancesMatrix[0]));

    QOpenGLExtraFunctions* gl = context()->extraFunctions();

    for (unsigned i = 0; i < 4; i++)
    {
        programInfo.program->enableAttributeArray(instanceLocation + i);
        programInfo.program->setAttributeBuffer(instanceLocation + i, GL_FLOAT, i * sizeof(QVector4D), 4, sizeof(m_instancesMatrix[0]));
        gl->glVertexAttribDivisor(instanceLocation + i, 1);
    }

    m_instanceBuffer.release();
    //programInfo.vbo.release();
    programInfo.vao.release();

    programInfo.program->bind();
    programInfo.program->setUniformValue("cameraMatrix", m_cameraMatrix);
    if (hasTexture)
    {
        programInfo.program->setUniformValue("texture", 0);
    }
    programInfo.program->release();

    return true;
}

void GlWindow::createProgramCubes()
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    const auto& cubeVertices = cube::vertices;
    const auto& cubeNormals = cube::normals;
    const auto& cubeTexcoords = cube::uvs;
    const auto& cubeIndices = cube::indices;
    vertices.resize(cubeVertices.size() * (3+3+2));
    indices.resize(cubeIndices.size());

    int index = 0;
    for (uint32_t i = 0; i < cubeVertices.size(); ++i)
    {
        //position
        vertices[index++] = cubeVertices[i].x();
        vertices[index++] = cubeVertices[i].y();
        vertices[index++] = cubeVertices[i].z();

        //normals
        vertices[index++] = cubeNormals[i].x();
        vertices[index++] = cubeNormals[i].y();
        vertices[index++] = cubeNormals[i].z();

        //texcoords
        vertices[index++] = cubeTexcoords[i].x();
        vertices[index++] = cubeTexcoords[i].y();

    }

    index = 0;
    for (uint32_t i = 0; i < cubeIndices.size(); ++i)
    {
        indices[index++] = cubeIndices[i];
    }

    if (!createProgram(vertices, indices, m_programs[ProgramType_Cubes]))
    {
        Q_ASSERT(false);
    }
}


void GlWindow::initializeGL()
{
    initializeOpenGLFunctions();

    //makeCurrent();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glFrontFace(GL_CCW);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Calculate aspect ratio
    m_aspectRatio = m_windowSizeX / m_windowSizeY;

    const std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    const std::string renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

    qDebug() << "version: "
        << QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    qDebug() << "GSLS version: "
        << QLatin1String(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    
    m_instancesMatrix.resize(m_initialInstanceCount);
    createMatrices(m_initialInstanceCount);

    m_cameraMatrix.setToIdentity();
    m_cameraMatrix.perspective(m_fov, m_aspectRatio, m_zNear, m_zFar);
    m_cameraMatrix.lookAt(m_cameraPosition, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });

    createProgramCubes();

    m_elapsedTimerFps->start();
    m_elapsedTimerDeltaTime->start();
    //const double targetFrametime = 1000.0 / 120.0; //1000.0 / 120.0;
    //m_repaintTimer->start(targetFrametime);
}

void GlWindow::resizeGL(int w, int h)
{
    m_windowSizeX = w;
    m_windowSizeY = h;
    m_aspectRatio = m_windowSizeX / m_windowSizeY;
    m_cameraMatrix.setToIdentity();
    m_cameraMatrix.perspective(m_fov, m_aspectRatio, m_zNear, m_zFar);
    m_cameraMatrix.lookAt(m_cameraPosition, { 0.0,0.0,0.0 }, { 0.0,1.0,0.0 });

    update();
}

void GlWindow::paintGL()
{
    m_elapsedTimerDeltaTime->start();

    animate(m_deltaTime);
    rotateCamera(m_deltaTime);

    // this function is called for every frame to be rendered on screen
    const qreal retinaScale = devicePixelRatio(); // needed for Macs with retina display
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    // set the background color = clear color
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawProgram(ProgramType_Cubes);

    const double elapsed = m_elapsedTimerFps->nsecsElapsed() * 0.000000001;
    if (elapsed >= 1.0)
    {
        m_fps = std::round(m_frameCount / elapsed);
        emit fpsUpdated(m_fps);    
        m_frameCount = 0;
        m_elapsedTimerFps->restart();
   
        //uint32_t newInstances = m_newInstancesPerTick;// (m_instancesMatrix.size() * m_instancesMatrix.size()) - m_instancesMatrix.size();
        //createMatrices(newInstances);

        qDebug() << "GlWindow FPS: " << m_fps << " deltatime: " << m_deltaTime << " count: " << m_instancesMatrix.size() << " m_newInstancesPerTick: " << m_newInstancesPerTick;
        //m_newInstancesPerTick += 100;
    }

    m_deltaTime = m_elapsedTimerDeltaTime->nsecsElapsed() * 0.000000001;

    ++m_frameCount;

    glFinish();
}

void GlWindow::drawProgram(ProgramType type)
{
    if (m_programs.find(type) != m_programs.end())
    {
        m_programs[type].program->bind();
        if (m_programs[type].texture)
        {
            m_programs[type].texture->bind();
        }

        m_programs[type].vao.bind();

        m_programs[type].program->setUniformValue("cameraMatrix", m_cameraMatrix);

        m_instanceBuffer.bind();
        m_instanceBuffer.allocate(m_instancesMatrix.data(), m_instancesMatrix.size() * sizeof(m_instancesMatrix[0]));
        //m_instanceBuffer.write(0, m_instancesMatrix.data(), m_instancesMatrix.size() * sizeof(m_instancesMatrix[0]));

        QOpenGLExtraFunctions* gl = context()->extraFunctions();
        gl->glDrawElementsInstanced(GL_TRIANGLES, m_programs[type].indicesCount, GL_UNSIGNED_INT, 0, m_instancesMatrix.size());
        //glDrawElements(GL_TRIANGLES, m_programs[type].indicesCount, GL_UNSIGNED_INT, nullptr);

        m_programs[type].vao.release();
        m_programs[type].program->release();
    }
}

void GlWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::MouseButton::LeftButton)
    {
    }
    else if(event->button() == Qt::MouseButton::RightButton)
    {
    }
}

void GlWindow::mouseMoveEvent(QMouseEvent* event)
{
    m_mousePosition = event->pos();
}

//void GlWindow::timerEvent(QTimerEvent* e)
//{
//    //update();
//}

void GlWindow::createMatrices(const uint32_t newInstances)
{
    float scale = 2.0f;
    uint32_t x_max = 50;
    uint32_t y_max = 50;
    uint32_t z_max = 50;
    for (uint32_t i = 0; i < newInstances; ++i)
    {
        QMatrix4x4 matrix;
        int startIndex = m_instancesMatrix.size();
        int indexPerZ = startIndex % (x_max * z_max);
        float x = (indexPerZ % x_max) - x_max * 0.5;
        float y = (startIndex / (x_max * z_max)) - y_max * 0.25;
        float z = (indexPerZ / z_max) - z_max * 0.5;
        matrix.translate(x * scale, y * scale, z * scale);
        m_instancesMatrix.push_back(matrix);
    }
}

void GlWindow::animate(const double deltaTime)
{
    m_objectRotationAngle = 150.0f * deltaTime;
    for (uint32_t i = 0; i < m_instancesMatrix.size(); ++i)
    {
        //m_instancesMatrix[i].setToIdentity();
        //m_instancesMatrix[i].translate(i* 2.0f, 0.0f, 0.0f);
        m_instancesMatrix[i].rotate(m_objectRotationAngle, QVector3D(0.0, 1.0, 0.0));
    }  
}

void GlWindow::rotateCamera(const double deltaTime)
{
    m_cameraRotationAngle = 3.0f * deltaTime;
    m_cameraMatrix.rotate(m_cameraRotationAngle, QVector3D(0.0, 1.0, 0.0));
}
