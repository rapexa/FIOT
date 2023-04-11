from flask import Flask, request, jsonify
import CONFIG

app = Flask(__name__)

# config
app.config.update(
    SECRET_KEY = "secret!"
)

@app.route('/trigger/LPG_event/with/key/100')
def hello_world():

    ret = {"code" : "200" , "data" : "hello world :)"}
    return jsonify(ret)

@app.route('/',methods = ['POST', 'GET'])
def render_page_web():
    if request.method == 'POST':
        token = request.form['token']  
        data = request.form['data'] 
        print(token,data) 
        return "hello"
    return "bye"

if __name__ == "__main__":
    app.run("0.0.0.0",5511,debug=False)