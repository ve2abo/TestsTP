import sys
import random
from flask import Flask, jsonify, make_response, request, abort

app = Flask(__name__)

@app.after_request
def after_request(data):
	response = make_response(data)
	response.headers['Content-Type'] = 'application/json'
	response.headers['Access-Control-Allow-Origin'] = '*'
	response.headers['Access-Control-Allow-Headers'] = "Origin, X-Requested-With, Content-Type, Accept"
	response.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE'
	return response

tasks = []

@app.errorhandler(400)
def bad_request(error):
    return make_response(jsonify( { 'error': 'Bad Request' } ), 400)

@app.errorhandler(404)
def not_found(error):
    return make_response(jsonify( { 'error': 'Not Found' } ), 404)


@app.route('/tasks/<int:task_id>', methods = ['POST'])
def create_task(task_id):
    if not request.json or not 'task' in request.json:
        abort(400)
    task = {
        'id': task_id,
        'task': request.json['task']
    }
    tasks.append(task)
    return jsonify( { 'tasks': tasks } ), 201

@app.route('/tasks', methods = ['GET'])
def get_tasks():
    return jsonify( { 'tasks': tasks } )
	
@app.route('/tasks/<int:task_id>', methods = ['GET'])
def get_task(task_id):
    task = filter(lambda t: t['id'] == task_id, tasks)
    if len(task) == 0:
        abort(404)
    return jsonify( { 'task': task[0] } )
	
@app.route('/tasks/<int:task_id>', methods = ['PUT'])
def update_task(task_id):
    task = filter(lambda t: t['id'] == task_id, tasks)
    if len(task) == 0 or not request.json:
        abort(404)
    if 'task' in request.json and type(request.json['task']) is not unicode:
        abort(400)
    task[0]['task'] = request.json.get('task', task[0]['task'])
    return jsonify( { 'tasks': tasks } )

@app.route('/tasks/<int:task_id>', methods = ['DELETE'])
def delete_task(task_id):
    task = filter(lambda t: t['id'] == task_id, tasks)
    if len(task) == 0:
        abort(404)
    tasks.remove(task[0])
    return jsonify( { 'tasks': tasks } )

if __name__ == '__main__':
    app.run(debug = True)