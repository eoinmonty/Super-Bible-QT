#include "glwidget.h"

// Random number generator
static unsigned int seed = 0x13371337;


static inline float random_float()
{
    float res;
    unsigned int tmp;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);

    *((unsigned int *) &res) = (tmp >> 9) | 0x3F800000;

    return (res - 1.0f);
}

// For generating initial position and velocity data
static void GenerateInitialPositions(float * position_data, unsigned int num_positions, float scale, const float *bias)
{
    unsigned int n;
    float * p = position_data;

    if (bias)
    {
        for (n = 0; n < num_positions; n++) {
            *p++ = (random_float() - 0.5f) * scale + bias[0];
            *p++ = (random_float() - 0.5f) * scale + bias[1];
            *p++ = (random_float() - 0.5f) * scale + bias[2];
        }
    }
    else
    {
        for (n = 0; n < num_positions; n++) {
            *p++ = (random_float() - 0.5f) * scale;
            *p++ = (random_float() - 0.5f) * scale;
            *p++ = (random_float() - 0.5f) * scale;
        }
    }
}


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    flock_size = 1500;
    done = false;
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    QTimer *timer = new QTimer(this);
    timer->setInterval(1);

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    setAutoBufferSwap(false);


    timer->start();
#ifndef Q_WS_QWS
    setMinimumSize(300, 250);
#endif
}

GLWidget::~GLWidget()
{
}

void GLWidget::resizeGL(int w, int h)
{
    // Prevent a divide by zero
    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 5000.0f);

    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

}

void GLWidget::paintGL()
{
    static GLuint frame_index = 0;
    static CStopWatch stopwatch;
    static float last_update_time = 0.0f;

    // Static state setup
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Do the position update
    glUseProgram(flockingUpdateProgram);

    // Update time based uniforms (location of shared goal and the simulation timestep)
    float t = stopwatch.GetElapsedSeconds();
    float goal_position[3];

    goal_position[0] = 240.0f * sinf(t * 1.0f * 3.141592f);
    goal_position[1] = 240.0f * cosf(t * 3.0f * 3.141592f);
    goal_position[2] = 240.0f * cosf(t * 2.5f * 3.141592f);

    glUniform3fv(goal_location, 1, goal_position);
    glUniform1f(timestep_loc, (t - last_update_time) * 50.0f);
    last_update_time = t;

    // Depending on whether we're rendering an even or odd frame...
    if (frame_index & 1)
    {
        // Read from second set of buffers
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, velocity_tbo[1]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, velocity_tbo[1]);
        glBindVertexArray(update_vao[1]);

        // Write to first
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, flock_position[0]);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, flock_velocity[0]);
    }
    else
    {
        // Read from first set of buffers
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, velocity_tbo[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_BUFFER, position_tbo[0]);
        glBindVertexArray(update_vao[0]);

        // Write to second
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, flock_position[1]);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, flock_velocity[1]);
    }

    // Turn off rasterization for the simulation pass (no fragment shader)
//glEnable(GL_RASTERIZER_DISCARD);

    // Press record...
    glBeginTransformFeedback(GL_POINTS);
    // Draw
    glDrawArrays(GL_POINTS, 0, flock_size);
    // Press stop...
    glEndTransformFeedback();

    // Turn rasterization back on
    glDisable(GL_RASTERIZER_DISCARD);

    // Now, we'll render the result - clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the rendering program
    glUseProgram(flockingRenderProgram);

    // Set the projection matrix
    modelViewMatrix.PushMatrix(viewFrame);

    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());

    modelViewMatrix.PopMatrix();

    // Depending on even or odd frame...
    if (frame_index & 1) {
        glBindVertexArray(render_vao[1]);
    } else {
        glBindVertexArray(render_vao[0]);
    }

    // Draw the appropriate set of data. This is an instanced draw. The
    // flock positions are now instanced and the geometry rendered is the
    // little airplane model.
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 8, flock_size);

    // Update the frame counter
    frame_index++;


    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    int i;

    // This is position and normal data for a paper airplane
    static const GLfloat geometry[] =
    {
        // Positions
        -5.0f, 1.0f, 0.0f,
        -1.0f, 1.5f, 0.0f,
        -1.0f, 1.5f, 7.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 10.0f,
        1.0f, 1.5f, 0.0f,
        1.0f, 1.5f, 7.0f,
        5.0f, 1.0f, 0.0f,

        // Normals
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
    };
    float * position_data;

    // Set the center of our projection to 2000 units out.
    // Move this further out if you have a really big flock.
    viewFrame.MoveForward(2000.0f);

    // Create the VAOs used for the update and render passes
    glGenVertexArrays(2, update_vao);
    glGenVertexArrays(2, render_vao);

    // Two buffers for each of position and velocity, one for the
    // airplane geometry
    glGenBuffers(2, flock_position);
    glGenBuffers(2, flock_velocity);
    glGenBuffers(1, &flock_geometry);

    // Prime the position buffers
    position_data = new float [flock_size * 3];

    static const float flock_center[] = { 0.0f, 100.0f, 50.0f };
    GenerateInitialPositions(position_data, flock_size, sqrt((float)flock_size) * 5.0f, flock_center);

    // Setup Update VAO 1
    // Positions first
    glBindVertexArray(update_vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, flock_position[0]);
    glBufferData(GL_ARRAY_BUFFER, flock_size * 3 * sizeof(float), position_data, GL_DYNAMIC_COPY);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    // Now velocity (give each flock member a small initial velocity)
    GenerateInitialPositions(position_data, flock_size, 0.005f, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, flock_velocity[0]);
    glBufferData(GL_ARRAY_BUFFER, flock_size * 3 * sizeof(float), position_data, GL_DYNAMIC_COPY);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // Done with data (everything's on the GPU from now on)
    delete [] position_data;

    // Setup Update VAO 2
    glBindVertexArray(update_vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, flock_position[1]);
    glBufferData(GL_ARRAY_BUFFER, flock_size * 3 * sizeof(float), NULL, GL_DYNAMIC_COPY);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, flock_velocity[1]);
    glBufferData(GL_ARRAY_BUFFER, flock_size * 3 * sizeof(float), NULL, GL_DYNAMIC_COPY);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    // Setup Render VAOs
    for (i = 0; i < 2; i++)
    {
        glBindVertexArray(render_vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, flock_position[i]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);
        glVertexAttribDivisor(0, 1);
        glBindBuffer(GL_ARRAY_BUFFER, flock_velocity[i]);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);
        glVertexAttribDivisor(1, 1);
        glBindBuffer(GL_ARRAY_BUFFER, flock_geometry);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
        // Only allocate the static geometry buffer once
        if (i == 0)
            glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);
    }

    // Setup position and velocity TBOs
    glGenTextures(2, position_tbo);
    glGenTextures(2, velocity_tbo);

    // Done with VAOs
    glBindVertexArray(0);

    // Set up TBOs
    for (i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_BUFFER, position_tbo[i]);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, flock_position[i]);
        glBindTexture(GL_TEXTURE_BUFFER, velocity_tbo[i]);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, flock_velocity[i]);
    }

    // Generate update program
    flockingUpdateProgram = glCreateProgram();

    // Load and compile the vertex shader
    update_vs = glCreateShader(GL_VERTEX_SHADER);
    gltLoadShaderFile("flocking_update.vs", update_vs);

    glCompileShader(update_vs);

    glAttachShader(flockingUpdateProgram, update_vs);

    // Setup transform feedback varyings
    static const GLchar * tf_outputs[] = { "position_out", "velocity_out" };

    glTransformFeedbackVaryings(flockingUpdateProgram, 2, tf_outputs, GL_SEPARATE_ATTRIBS);

    // Link. Notice no fragment shader.
    glLinkProgram(flockingUpdateProgram);
    glUseProgram(flockingUpdateProgram);

    // Setup the sampler uniforms (can't initalize defaults in the shader)
    glUniform1i(glGetUniformLocation(flockingUpdateProgram, "tex_positions"), 0);
    glUniform1i(glGetUniformLocation(flockingUpdateProgram, "tex_velocities"), 1);
    // Set the flock size
    glUniform1i(glGetUniformLocation(flockingUpdateProgram, "flock_size"), flock_size);
    // Get the location of the timestep uniform
    timestep_loc = glGetUniformLocation(flockingUpdateProgram, "timestep");
    goal_location = glGetUniformLocation(flockingUpdateProgram, "goal");

    glUseProgram(0);

    // Generate render program
    flockingRenderProgram = glCreateProgram();

    // Load and compile VS, GS and FS
    render_vs = glCreateShader(GL_VERTEX_SHADER);
    render_gs = glCreateShader(GL_GEOMETRY_SHADER);
    render_fs = glCreateShader(GL_FRAGMENT_SHADER);

    gltLoadShaderFile("flocking_render.vs", render_vs);
    gltLoadShaderFile("flocking_render.gs", render_gs);
    gltLoadShaderFile("flocking_render.fs", render_fs);

    glCompileShader(render_vs);
    glCompileShader(render_gs);
    glCompileShader(render_fs);

    glAttachShader(flockingRenderProgram, render_vs);
    glAttachShader(flockingRenderProgram, render_gs);
    glAttachShader(flockingRenderProgram, render_fs);

    // Link
    glLinkProgram(flockingRenderProgram);
    glUseProgram(flockingRenderProgram);

    mvp_location = glGetUniformLocation(flockingRenderProgram, "mvp");


}

void GLWidget::keyPressEvent(QKeyEvent *e)
{

    switch( e->key() )
    {
        case Qt::Key_Escape:
            done = true;
            break;
        default:
            break;
    }
    updateGL();
}
