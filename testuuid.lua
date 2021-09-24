local uuid = require 'uuid'

local function test_1 ()
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
end

local function test_2 ()
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
end

local function test_3 ()
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
end

print 'test_1'
test_1()

print 'test_2'
test_2()

print 'test_3'
test_3()
