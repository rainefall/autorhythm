extends Node;

# "cubic array" used to interpolate values for the colour of the blocks/highway
var intensity_array;
# "cubic array" used to interpolate values for the Y position of blocks
var height_array;

# zero matrix for "removing" a block
var transform_zero = Transform(Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0),Vector3(0,0,0));

# index of next block that the player can collide with
var next_block = 0;

# score
var score = 0;
var score2 = 0;
# score multiplier
var score_multiplier = 1.0;
var score_multiplier2 = 1.0;

# mesh for the level curve
var level_mesh: ArrayMesh;

# object that stores the FMOD Sound and FMOD Channel objects required
# to play the music
var sound;

# is the game paused or not
var paused_state = false;

# Called when the node enters the scene tree for the first time.
func _ready():
	# load song
	sound = Global.FMODSound.new();
	sound.create(Global.current_lvl_path);
	
	# create intensity/height cubic arrays and level geometry (the curve)
	intensity_array = Global.CubicArray.new();
	height_array = Global.CubicArray.new();
	# vertex x position for building the level geometry 
	var vertexx;
	if Global.two_player_mode:
		vertexx = 8;
	else:
		vertexx = 4;
	var h = 0.0;
	var st = SurfaceTool.new();
	st.begin(Mesh.PRIMITIVE_TRIANGLES);
	for i in range(0, Global.current_lvl["shape"].size()):
		var normalized = (Global.current_lvl["shape"][i] - Global.current_lvl["shape"].min()) / (Global.current_lvl["shape"].max() - Global.current_lvl["shape"].min());
		
		# building level geometry
		# first tri
		# 441 = distance travelled in 5 seconds
		st.add_uv(Vector2(0.5,0));
		st.add_vertex(Vector3(-vertexx, h, 441 * (i-1)));
		st.add_uv(Vector2(vertexx + 0.5,0));
		st.add_vertex(Vector3(vertexx, h, 441 * (i-1)));
		st.add_uv(Vector2(vertexx + 0.5,0));
		st.add_vertex(Vector3(vertexx, h - normalized * 50, 441 * i));
		# new value for h
		h -= normalized * 50;
		# 441 = distance travelled in 5 seconds
		st.add_uv(Vector2(0.5,0));
		st.add_vertex(Vector3(-vertexx, h + normalized * 50, 441 * (i-1)));
		st.add_uv(Vector2(vertexx + 0.5,0));
		st.add_vertex(Vector3(vertexx, h, 441 * i));
		st.add_uv(Vector2(0.5,0));
		st.add_vertex(Vector3(-vertexx, h, 441 * i));
		
		# push to intensity array
		# intensity values are 1 every 5 seconds (or 1 every 44100 * 5 samples)
		# value is the onsets per second mapped from 0 to 1, 0 representing lowest in the track 1 representing highest in the track
		intensity_array.add_value((i+1) * 44100 * 5, normalized);
		height_array.add_value(i * 441, h);
	level_mesh = st.commit();
	$LevelGeometry.mesh = level_mesh;
	
	# place blocks
	$Blocks.multimesh.instance_count = Global.current_lvl["onsets"].size() / 12;
	# currently, this does not work as it sets transform data incorrectly
	#$Blocks.multimesh.set_as_bulk_array(Global.current_lvl["onsets"])
	
	# this is HORRIBLY inefficient
	# but godot has forced my hand, as set_as_bulk_array does not work
	for i in range(0,Global.current_lvl["onsets"].size()/12):
		var offset = i*12;
		var t = Transform(	Vector3(Global.current_lvl["onsets"][offset],Global.current_lvl["onsets"][offset+1],Global.current_lvl["onsets"][offset+2]),
							Vector3(Global.current_lvl["onsets"][offset+3],Global.current_lvl["onsets"][offset+4],Global.current_lvl["onsets"][offset+5]),
							Vector3(Global.current_lvl["onsets"][offset+6],Global.current_lvl["onsets"][offset+7],Global.current_lvl["onsets"][offset+8]),
							Vector3(Global.current_lvl["onsets"][offset+9],Global.current_lvl["onsets"][offset+10],Global.current_lvl["onsets"][offset+11]));
		t.origin.y = height_array.get_value(t.origin.z);
		$Blocks.multimesh.set_instance_transform(i, t);
	
	# set initial UI values
	get_node("User Interface/Multiplier").text = "x1.0";
	get_node("User Interface/Score").text = "0";
	get_node("User Interface/Metadata").text = Global.current_lvl["metadata"][1] + " - " + Global.current_lvl["metadata"][0];
	
	if Global.two_player_mode:
		$Blocks2.multimesh = $Blocks.multimesh;
	
	# play the music
	sound.play();

# godot doesnt let you directly delete things from an multimesh
# however, setting the instance's transform to be all zeroes makes it disappear
func remove_block(index, mmesh):
	mmesh.set_instance_transform(index,transform_zero);

# Called once every frame
func _process(delta):
	if paused_state:
		# check for pause button, unpause
		if Input.is_action_just_pressed("game_pause"):
			$"User Interface/PauseMenu".hide();
			sound.unpause();
			paused_state = false;
	else:
		# check for pause button, pause
		if Input.is_action_just_pressed("game_pause"):
			$"User Interface/PauseMenu".show();
			sound.pause();
			paused_state = true;
		
		$Blocks.material_override.set_shader_param("position", intensity_array.get_value(sound.channel_position()));
		if Global.two_player_mode:
			$Blocks2.material_override.set_shader_param("position", intensity_array.get_value(sound.channel_position()));
		$LevelGeometry.material_override.set_shader_param("position", intensity_array.get_value(sound.channel_position()));
		
		# move camera
		$Camera.transform.origin.z = $Player.transform.origin.z - 8;
		$Camera.transform.origin.y = $Player.transform.origin.y + 4;
		var rotation = -10-rad2deg(atan((50 * intensity_array.get_value(sound.channel_position())) / 441))
		$Camera.set_rotation_degrees(Vector3(rotation,-180,0))
		
		if sound.channel_position() >= Global.current_lvl["metadata"][2] and next_block >= Global.current_lvl["onsets"].size() / 12:
			Global.save_score(score, Global.game_settings["defname"]);
			if Global.two_player_mode:
				Global.save_score(score2, Global.game_settings["defname2"]);
			get_tree().change_scene("res://scenes/highscore.tscn");
		
		if next_block < Global.current_lvl["onsets"].size() / 12:
			var hits = 0;
			if $Player.transform.origin.z > Global.current_lvl["onsets"][next_block * 12 + 11] + 2:
				# very much not in range
				# reset score multiplier to 1
				score_multiplier = 1.0;
				
				# increase block counter if not in 2 player mode
				if !Global.two_player_mode:
					next_block += 1;
				
				# update UI
				get_node("User Interface/Multiplier").text = "x"+str(score_multiplier)
			elif $Player.transform.origin.z > Global.current_lvl["onsets"][next_block * 12 + 11] - 2.5:
				if $Player.transform.origin.x - $Player.origin > Global.current_lvl["onsets"][next_block * 12 + 9] - 1.5 && $Player.transform.origin.x - $Player.origin < Global.current_lvl["onsets"][next_block * 12 + 9] + 1.5:
					# "remove" block so that the player knows they have hit it
					remove_block(next_block, $Blocks.multimesh);
					# increase score and multiplier
					score += 10 * intensity_array.get_value(sound.channel_position()) * score_multiplier;
					if score_multiplier < 4.0:
						score_multiplier += 0.1;
					
					# hit a block
					hits += 1;
					
					# update UI
					get_node("User Interface/Multiplier").text = "x"+str(score_multiplier);
					get_node("User Interface/Score").text = str(int(score));
					
			# 2nd player collision checks
			if Global.two_player_mode:
				if $Player2.transform.origin.z > Global.current_lvl["onsets"][next_block * 12 + 11] + 2:
					# very much not in range
					# reset score multiplier to 1
					score_multiplier2 = 1.0;
					
					# update UI
					get_node("User Interface/Multiplier2").text = "x"+str(score_multiplier2);
					
					# increase block check counter
					next_block += 1;
					
				elif $Player2.transform.origin.z > Global.current_lvl["onsets"][next_block * 12 + 11] - 2.5:
					if $Player2.transform.origin.x - $Player2.origin > -Global.current_lvl["onsets"][next_block * 12 + 9] - 1.5 && $Player2.transform.origin.x - $Player2.origin < -Global.current_lvl["onsets"][next_block * 12 + 9] + 1.5:
						# "remove" block so that the player knows they have hit it
						remove_block(next_block, $Blocks2.multimesh);
						# increase score and multiplier
						score2 += 10 * intensity_array.get_value(sound.channel_position()) * score_multiplier2;
						if score_multiplier2 < 4.0:
							score_multiplier2 += 0.1;
							
						# hit a block
						hits += 1;
						
						# update UI
						get_node("User Interface/Multiplier2").text = "x"+str(score_multiplier2);
						get_node("User Interface/Score2").text = str(int(score2));
						
			if hits > 0:	
				# increase block check counter
				next_block += 1;

func _on_Resume_pressed():
	$"User Interface/PauseMenu".hide();
	sound.unpause();
	paused_state = false;


func _on_Exit_pressed():
	get_tree().change_scene("res://scenes/Menus.tscn");
