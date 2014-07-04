
#include "GameEngine.hpp"

int             read_from_server(t_selfd *fd)
{
    char        buff[BUFSIZ];
    int         r;
    size_t      size;

    r = 0;
    size = ring_buffer_left_write(fd->rbuff);
    size = (size < sizeof(buff)) ? size : sizeof(buff);
    if (size && ((r = read(fd->fd, buff, size)) > 0))
        write_buffer(fd->rbuff, buff, r);
    return (r);
}

std::string               get_command(t_selfd *fd)
{
    char                  *cmd;
    size_t                size;
    char                  buff[512];

    size = read_buffer(fd->rbuff, buff, sizeof(buff));
    if (size && ((cmd = static_cast<char *>(memchr(buff, '\n', size))))) {
        rollback_read_buffer(fd->rbuff, size - (cmd - buff + 1));
        buff[(cmd - buff)] = '\0';
        return (std::string(buff));
    }
    return (std::string(""));
}

GameEngine::GameEngine(const int &x, const int &y)
: _window(sf::VideoMode(x, y), WINDOW_NAME, sf::Style::Default, sf::ContextSettings(32, 8, 3, 3, 0))
{
    _window.setFramerateLimit(FPS);

    _cube = new Cube;
    _cube->build();
    _cube->loadTexture("res/textures/grass.png");

//    for (int y = 0; y < 100; ++y) {
//        for (int x = 0; x < 100; ++x) {
//            _map.push_back(new Cube(*_cube));
//            _map.back()->translate(glm::vec3(x, 0, y));
//        }
//    }

//    run();

    /* Init connexion */
    _client = create_connection("lodow.net", "4242", SOCK_STREAM, &connect_nb);
    if (!_client)
        return ;
    int status;
    std::cout << "Connecting . " << std::endl;
    while ((status = is_connected(_client)) == 1) {
        usleep(500);
        std::cout << ".";
    }
    if (status == -1) {
        std::cerr << "Error on connection" << std::endl;
        return ;
    }
    _elem = NULL;

    add_to_list(&_elem, static_cast<void *>(create_fd(_client->socket, NULL, (int (*)())(&handle_server))));

    _tv.tv_sec = 0;
    _tv.tv_usec = 100000;

    _parser = new Parser(&_map, _cube);
    do_select(_elem, &_tv, _parser);
    write(_client->socket, "GRAPHIC\n", 8);

    run();
}

GameEngine::~GameEngine()
{
    delete _parser;
    delete _cube;
}

void	GameEngine::run() {

    glEnable(GL_DEPTH_TEST);

    Shader *shader = new Shader("res/shaders/basic.vert", "res/shaders/basic.frag");
    Camera camera;

    camera.setPos(glm::vec3(0, 3, 2));
    camera.setPointView(glm::vec3(0, 0, 0));
    shader->initialiser();

    while (_window.isOpen()) {
        sf::Event event;
        while (_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                event.key.code == sf::Keyboard::Escape)
                _window.close();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.lookAt();
        shader->bind();
        shader->setUniform("projection", camera.getProjection());
        shader->setUniform("view", camera.getTransformation());

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            camera.translate(glm::vec3(0, 0, -0.1));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            camera.translate(glm::vec3(0, 0, 0.1));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            camera.translate(glm::vec3(-0.1, 0, 0));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            camera.translate(glm::vec3(0.1, 0, 0));

        do_select(_elem, &_tv, _parser);

        for (Map::iterator it = _map.begin(), end = _map.end(); it != end; ++it)
        {
            (*it)->draw(shader);
            //            std::cout << (*it)->getPos().x << std::endl;
        }
        _window.display();
    }
    delete shader;
}

int handle_server(t_selfd *fd, void *parser)
{
    std::string cmd;
    int r;
    ssize_t swr;
    Parser *cpy = static_cast<Parser *>(parser);

    if (ISREADABLE(fd)) {
        if (((r = read_from_server(fd)) < 0 && errno != EINTR) || (r == 0))
            return (-1);
    }
    //  if (ISWRITEABLE(fd) && (r = write_to_client(fd)) < 0 && errno != EINTR)
    //    return (-1);
    while ((cmd = get_command(fd)) != "")
        cpy->parseCmd(cmd);
    swr = ring_buffer_left_read(fd->wbuff);
    if (!swr && fd->to_close)
        return (-1);
    if (swr)
        CHECKWRITE(fd);
    CHECKREAD(fd);
    return (0);
}