from propnet.propnet_wrapper import PropnetWrapper

from http.server import BaseHTTPRequestHandler, HTTPServer

HOST = "localhost"
PORT = 8080
URL = f"http://{HOST}:{PORT}"

def main() -> None:
    server = create_server("some_game")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        server.server_close()

def create_server(game: str) -> HTTPServer:
    def create_handler(*args):
        return GameServerHandler(game, *args)

    server = HTTPServer((HOST, PORT), create_handler)
    print(f"Created game server at {URL}")
    return server

class GameServerHandler(BaseHTTPRequestHandler):
    def __init__(self, game: str, *args):
        self.game = game
        self.propnet_wrapper = PropnetWrapper(game)
        BaseHTTPRequestHandler.__init__(self, *args)

    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        self.wfile.write(bytes("<html><head><title>https://pythonbasics.org</title></head>", "utf-8"))
        self.wfile.write(bytes("<p>Request: %s</p>" % self.path, "utf-8"))
        self.wfile.write(bytes("%s" % self.game, "utf-8"))
        self.wfile.write(bytes("<body>", "utf-8"))
        self.wfile.write(bytes("<p>This is an example web server.</p>", "utf-8"))
        self.wfile.write(bytes("</body></html>", "utf-8"))

def play_game():
    pass

if __name__ == '__main__':
    main()
