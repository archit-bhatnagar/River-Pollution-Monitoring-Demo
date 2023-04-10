# Import flask and datetime module for showing date and time
from flask import Flask, render_template
import os

template_dir = os.path.relpath("..\\web\\build")
static_dir = os.path.relpath("..\\web\\build\\static")

# Initializing flask app
app = Flask(__name__, template_folder=template_dir, static_folder=static_dir)


# Route for seeing a data
@app.route('/')
def home():
	# Returning the reactjs final build index.html page 
	return render_template("index.html")


# Running app
if __name__ == '__main__':
	app.run(debug=True, port=5000)
