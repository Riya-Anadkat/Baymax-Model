white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 8)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 8)
rootnode = gr.node('root')

torso = gr.mesh('sphere', 'torso')
torso:scale(1.0, 1.4, 0.9)
torso:translate(0.0, 0.0, -5.0)
torso:set_material(white)
rootnode:add_child(torso)

shouldersJoint = gr.joint('shouldersJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rootnode:add_child(shouldersJoint)

shoulders = gr.mesh('sphere', 'shoulders')
shoulders:scale(0.9, 0.9, 0.9)
shoulders:translate(0.0, 0.5, -5.05)
shoulders:set_material(white)
shouldersJoint:add_child(shoulders)

tummy = gr.mesh('sphere', 'tummy')
tummy:scale(1.1, 1.2, 1.0)
tummy:translate(0.0, -0.3, -5.0)
tummy:set_material(white)
torso:add_child(tummy)

neckJoint = gr.joint('neckJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
shouldersJoint:add_child(neckJoint)

head = gr.mesh('sphere', 'head')
head:scale(0.45, 0.35, 0.3)
head:translate(0.0, 1.6, -5.0)
head:set_material(white)
neckJoint:add_child(head)

rightEye = gr.mesh('sphere', 'rightEye')
rightEye:scale(0.05, 0.05, 0.05)
rightEye:translate(0.2, 1.6, -4.7)
rightEye:set_material(black)
head:add_child(rightEye)

leftEye = gr.mesh('sphere', 'leftEye')
leftEye:scale(0.05, 0.05, 0.05)
leftEye:translate(-0.15, 1.6, -4.7)
leftEye:set_material(black)
head:add_child(leftEye)

line = gr.mesh('sphere', 'line')
line:scale(0.20, 0.01, 0.05)
line:translate(0.0, 1.6, -4.7)
line:set_material(black)
head:add_child(line)

rightArmJoint = gr.joint('rightArmJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
shouldersJoint:add_child(rightArmJoint)

rightArm = gr.mesh('sphere', 'rightArm')
rightArm:scale(0.30, 0.8, 0.4)
rightArm:translate(1.1, -0.1, -5.0)
rightArm:rotate('z', 30)
rightArm:set_material(white)
rightArmJoint:add_child(rightArm)

rightLowerArmJoint = gr.joint('rightLowerArmJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rightArmJoint:add_child(rightLowerArmJoint)

rightLowerArm = gr.mesh('sphere', 'rightLowerArm')
rightLowerArm:scale(0.40, 0.75, 0.4)
rightLowerArm:translate(1.1, -0.6, -5.0)
rightLowerArm:rotate('z', 10)
rightLowerArm:set_material(white)
rightLowerArmJoint:add_child(rightLowerArm)


rightThumbJoint = gr.joint('rightThumbJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rightLowerArmJoint:add_child(rightThumbJoint)

rightThumb = gr.mesh('sphere', 'rightThumb')
rightThumb:scale(0.07, 0.20, 0.15)
rightThumb:translate(1.4, -0.45, -5)
rightThumb:set_material(white)
rightThumb:rotate('z', -25)
rightThumbJoint:add_child(rightThumb)

rightIndexFingerJoint = gr.joint('rightIndexFingerJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rightLowerArmJoint:add_child(rightIndexFingerJoint)

rightIndexFinger = gr.mesh('sphere', 'rightIndexFinger')
rightIndexFinger:scale(0.07, 0.20, 0.15)
rightIndexFinger:rotate('z', -30)
rightIndexFinger:translate(1.30, -1.15, -5)
rightIndexFinger:set_material(white)
rightIndexFingerJoint:add_child(rightIndexFinger)

rightMiddleFingerJoint = gr.joint('rightMiddleFingerJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rightLowerArmJoint:add_child(rightMiddleFingerJoint)

rightMiddleFinger = gr.mesh('sphere', 'rightMiddleFinger')
rightMiddleFinger:scale(0.05, 0.20, 0.15)
rightMiddleFinger:rotate('z', -25)
rightMiddleFinger:translate(1.28, -1.08, -5.1)
rightMiddleFinger:set_material(white)
rightMiddleFingerJoint:add_child(rightMiddleFinger)

rightRingFingerJoint = gr.joint('rightRingFingerJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rightLowerArmJoint:add_child(rightRingFingerJoint)

rightRingFinger = gr.mesh('sphere', 'rightRingFinger')
rightRingFinger:scale(0.05, 0.20, 0.15)
rightRingFinger:rotate('z', -20)
rightRingFinger:translate(1.28, -1.08, -5.2)
rightRingFinger:set_material(white)
rightRingFingerJoint:add_child(rightRingFinger)

rightThighJoint = gr.joint('rightThighJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rootnode:add_child(rightThighJoint)

rightThigh = gr.mesh('sphere', 'rightThigh')
rightThigh:scale(0.45, 0.9, 0.5)
rightThigh:rotate('z', -5)
rightThigh:translate(0.45, -1.5, -5.0)
rightThigh:set_material(white)
rightThighJoint:add_child(rightThigh)

rightKneeJoint = gr.joint('rightKneeJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rightThighJoint:add_child(rightKneeJoint)

rightKnee = gr.mesh('sphere', 'rightKnee')
rightKnee:scale(0.4, 0.7, 0.6)
rightKnee:rotate('z', -5)
rightKnee:translate(0.4, -1.9, -5.0)
rightKnee:set_material(white)
rightKneeJoint:add_child(rightKnee)

leftThighJoint = gr.joint('leftThighJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rootnode:add_child(leftThighJoint)

leftThigh = gr.mesh('sphere', 'leftThigh')
leftThigh:scale(0.45, 0.9, 0.5)
leftThigh:rotate('z', 5)
leftThigh:translate(-0.45, -1.5, -5.0)
leftThigh:set_material(white)
leftThighJoint:add_child(leftThigh)

leftKneeJoint = gr.joint('leftKneeJoint', {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
leftThighJoint:add_child(leftKneeJoint)

leftKnee = gr.mesh('sphere', 'leftKnee')
leftKnee:scale(0.4, 0.7, 0.6)
leftKnee:rotate('z', 5)
leftKnee:translate(-0.4, -1.9, -5.0)
leftKnee:set_material(white)
leftKneeJoint:add_child(leftKnee)

return rootnode


