entity        = world:getEntity(entity_id)
velocity      = entity:getVelocity()
position      = entity:getPosition()
input_state   = input_handler:getInputState()
shoot         = entity:getShoot()
reload_timer  = entity:getReloadTimer()

if input_state:test(eIC_Shoot) and shoot.mag > 0 then
	if not shoot.shoot_pressed then
		bullet = world:createEntity()
		bullet:setParent(shoot.projectile)
		bullet:setVelocity(velocity.x + 10.0, velocity.y, velocity.z)
		bullet:setPosition(position.x + 1.0, position.y, position.z)
		bullet:addCubeMesh()
		shoot.shoot_pressed = true
		shoot.mag = shoot.mag - 1
	end
else
	shoot.shoot_pressed = false
end

if shoot.mag == 0 then
	reload_timer.acc = reload_timer.acc + delta_time
end

if reload_timer.acc > reload_timer.time then
	shoot.mag = shoot.mag + reload_timer.mag
	reload_timer.acc = 0.0
end
