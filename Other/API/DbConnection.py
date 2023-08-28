import psycopg2
from psycopg2 import OperationalError


class DB():
    def __init__(self, db_name, db_user, db_password, db_host, db_port):
        self.db_name = db_name
        self.db_user = db_user
        self.db_password = db_password
        self.db_host = db_host
        self.db_port = db_port

    def create_connection(db_name, db_user, db_password, db_host, db_port):
        connection = None
        try:
            connection = psycopg2.connect(
                database=db_name,
                user=db_user,
                password=db_password,
                host=db_host,
                port=db_port,
            )
            print("Connection to PostgreSQL DB successful")
        except OperationalError as e:
            print(f"The error '{e}' occurred")
        return connection


    def execute_query(connection, query):
        cursor = connection.cursor()
        try:
            cursor.execute(query)
            connection.commit()
            print("Query executed successfully")
            res = cursor.fetchall()
            print(f"Time: {res[0][0]}")



        except OperationalError as e:
            print(f"The error '{e}' occurred")


    db_name = "d3aat7u22tdlh2"
    db_user = "opvuxnhxcuwrfn"
    db_password = "5c5411e37343e4ec95f54763343a339ce375c64f2d81709921f74626c2554750"
    db_host = "ec2-3-223-242-224.compute-1.amazonaws.com"
    db_port = 5432

    connection = create_connection(db_name, db_user, db_password, db_host, db_port)

    query = "Select * from feedlog"
    execute_query(connection, query)
