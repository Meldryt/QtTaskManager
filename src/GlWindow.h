#pragma once

#include <QtOpenGL>
#include <QOpenGLWindow>
#include <QMouseEvent>
#include <QElapsedTimer>
#include <vector>
#include <map>

class World;

class GlWindow : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GlWindow(QWindow *parent = nullptr);
    ~GlWindow();

signals:
    void fpsUpdated(uint32_t fps);

public slots:

protected:
    //void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    //void timerEvent(QTimerEvent* e) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    //void paintEvent(QPaintEvent* event) override;

    //void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    enum ProgramType : uint8_t
    {
        ProgramType_Cubes = 0
    };

    struct ProgramInfo
    {
        uint32_t indicesCount{ 0 };
        QOpenGLVertexArrayObject vao;
        QOpenGLBuffer vbo;
        QOpenGLBuffer ibo;
        QOpenGLShaderProgram* program{ nullptr };
    };

    bool createProgram(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, ProgramInfo& programInfo);
    void createProgramCubes();
    void drawProgram(ProgramType type);

    void createMatrices(const uint32_t newInstances);
    void animate(const double deltaTime);
    void rotateCamera(const double deltaTime);

    std::map<ProgramType, ProgramInfo> m_programs;

    const double m_zNear{ 0.1 };
    const double m_zFar{ 1000.0 };
    const double m_fov{ 60.0 };
    double m_aspectRatio{ 1.0 };
    uint16_t m_windowSizeX{ 800 };
    uint16_t m_windowSizeY{ 600 };
    QVector3D m_cameraPosition{ 0.0f, 10.0f, -70.0f };
    QPoint m_mousePosition{ 0,0 };

    QMatrix4x4 m_cameraMatrix;
    float m_objectRotationAngle{ 0 };
    float m_cameraRotationAngle{ 0 };
    std::vector<QMatrix4x4> m_instancesMatrix;
    const uint32_t m_initialInstanceCount{ 20000 };
    uint32_t m_newInstancesPerTick{ 0 };
    QOpenGLBuffer m_instanceBuffer;
    QOpenGLVertexArrayObject m_instancesVao;
    //GLint m_matrixUniform{ 0 }

    QElapsedTimer* m_elapsedTimerFps{ nullptr };
    QElapsedTimer* m_elapsedTimerDeltaTime{ nullptr };
    QTimer* m_repaintTimer{ nullptr };
    uint32_t m_frameCount{ 0 };
    uint32_t m_fps{ 0 };
    double m_deltaTime{ 0.0 };
};
