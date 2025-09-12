import socket
import re
from objects import Ad_Socket_Object, Ad_String_Object


def create_server(raw_socket: Ad_Socket_Object):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(('127.0.0.1', 5003))
    sock.listen()
    raw_socket.sock = sock


def create_client(raw_socket: Ad_Socket_Object):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 5003))
    raw_socket.sock = sock


def accept_socket(raw_socket: Ad_Socket_Object):
    conn, addr = raw_socket.sock.accept()
    client_sock = Ad_Socket_Object(name="client", host="127.0.0.1", port=5003, is_active=True,
            is_forever=False, is_client=True, is_server=False, sock=None,
            conn=conn, addr=addr)
    return client_sock


def send_socket(raw_socket: Ad_Socket_Object, raw_string: Ad_String_Object):
    escaped_text = cleanup_unescaped_characters(raw_string.value)
    if raw_socket.conn:
        raw_socket.conn.sendall(escaped_text.encode())


def read_socket(raw_socket: Ad_Socket_Object):
    message = ""
    while True:
        if not raw_socket.sock:
            break
        data = raw_socket.sock.recv(1024)
        if data:
            message += data.decode()
        else:
            break
    result = Ad_String_Object(value=message)
    return result


def read_http(raw_socket: Ad_Socket_Object):
    message = ""
    while True:
        if not raw_socket.sock:
            break
        data = raw_socket.sock.recv(1024)
        if data:
            message += data.decode()
        else:
            break
    result = Ad_String_Object(value=message)
    return result


def close_socket(raw_socket: Ad_Socket_Object):
    if raw_socket and raw_socket.conn:
        raw_socket.conn.close()


def cleanup_unescaped_characters(raw_text: str):
    # TODO: fix this
    escaped_text = re.sub('\\\\r', '\r', raw_text)
    escaped_text = re.sub('\\\\n', '\n', escaped_text)
    return escaped_text
