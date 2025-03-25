from flask import Flask, request

app = Flask(__name__)

@app.route("/")
def receive_data():
    value = request.args.get("var")
    print("Received value:", value)
    return "We received value: " + str(value)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)