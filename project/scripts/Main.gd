extends Node

var scene;

# Called when the node enters the scene tree for the first time.
func _ready():
	# load level cache
	
	# start menu scene
	scene = add_child(load("res://scenes/Menus.tscn").instance())


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
