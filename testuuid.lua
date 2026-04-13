local uuid = require 'uuid'

local function test_1()
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
end

local function test_2()
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
end

local function test_3()
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
	local myuuid <close> = uuid.generate_random()
	print(myuuid:unparse())
end

local function test_4()
	local time <close> = uuid.generate_time()
	print(time)
	local time_v6 <close> = uuid.generate_time_v6()
	print(time_v6)
	local time_v7 <close> = uuid.generate_time_v7()
	print(time_v7)
end

print 'test_1'
test_1()

print 'test_2'
test_2()

print 'test_3'
test_3()

print 'test_4'
test_4()
