-- Variables
speed     = 15.0
jumpSpeed = 10.0
epsilon   = 0.1

deltaVelX = 0.0
deltaVelY = 0.0
deltaVelZ = 0.0

entity     = world:getEntity(entity_id)
velocity   = entity:getVelocity()
position   = entity:getPosition()
plane      = entity:getBouncePlane()
inputState = inputHandler:getInputState()

gun         = entity:getGun()
reloadTimer = entity:getReloadTimer()

-- Controlling the gun 
if inputState:test(eIC_GoLeft) then
	deltaVelX = deltaVelX - speed
end
if inputState:test(eIC_GoRight) then
	deltaVelX = deltaVelX + speed
end
if inputState:test(eIC_GoBackward) then
	deltaVelZ = deltaVelZ - speed
end
if inputState:test(eIC_GoForward) then
	deltaVelZ = deltaVelZ + speed
end
velNormSquared = deltaVelX * deltaVelX + deltaVelZ * deltaVelZ
if velNormSquared > speed * speed then
    deltaVelX = deltaVelX * 0.707
    deltaVelZ = deltaVelZ * 0.707
end
if inputState:test(eIC_Jump) then
	if plane.x * position.x + plane.y * position.y + plane.z * position.z < plane.w + epsilon then
		velocity.y = jumpSpeed
	end
end

velocity.x = velocity.x + deltaVelX * dt
velocity.z = velocity.z + deltaVelZ * dt

-- Firing from the gun
if inputState:test(eIC_Shoot) and gun.numberOfBullets > 0 then
	if not gun.shootKeyPressed then
		bullet = world:createEntity()
		bullet:setOwner(gun.bullet)
		bullet:setVelocity(velocity.x, velocity.y, velocity.z + 30.0)
		bullet:setPosition(position.x, position.y, position.z + 1.0)
		bullet:addIcosahedronMesh()
		gun.shootKeyPressed = true
		gun.numberOfBullets = gun.numberOfBullets - 1
	end
else
	gun.shootKeyPressed = false
end

if gun.numberOfBullets == 0 then
	reloadTimer.timeElapsed = reloadTimer.timeElapsed + dt
end

if reloadTimer.timeElapsed > reloadTimer.time then
	gun.numberOfBullets = gun.numberOfBullets + reloadTimer.numberOfBulletsToAdd
	reloadTimer.timeElapsed = 0.0
end
