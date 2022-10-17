speed         = 10.0
jump_speed    = 10.0;
plane_epsilon = 0.1;
delta_speed   = {}
delta_speed.x = 0.0
delta_speed.y = 0.0
delta_speed.z = 0.0
entity        = world:getEntity(entity_id)
velocity      = entity:getVelocity()
position      = entity:getPosition()
plane         = entity:getBouncePlane()
input_state   = input_handler:getInputState()

if input_state:test(eIC_GoLeft) then
	delta_speed.x = delta_speed.x - speed
end
if input_state:test(eIC_GoRight) then
	delta_speed.x = delta_speed.x + speed
end
if input_state:test(eIC_GoBackward) then
	delta_speed.z = delta_speed.z - speed
end
if input_state:test(eIC_GoForward) then
	delta_speed.z = delta_speed.z + speed
end
if input_state:test(eIC_Jump) then
	if plane.x * position.x + plane.y * position.y + plane.z * position.z < plane.w + plane_epsilon then
		velocity.y = jump_speed
	end
end

velocity.x = velocity.x + delta_speed.x * delta_time
velocity.z = velocity.z + delta_speed.z * delta_time
