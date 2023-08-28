import flask
from flask import jsonify

app = flask.Flask(__name__)
app.config["DEBUG"] = True


@app.route('/feeds', methods=['GET'])
def home():
    data = {'name': 'nabin khadka'}
    return jsonify(data)

@app.route('/feeds/latest', methods=['GET'])
def home():
    data = {'name': 'nabin khadka'}
    return jsonify(data)

@app.route('/s', methods=['POST'])
def home2():
    return "Slmog"

app.run(port=3000)

