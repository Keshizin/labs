-- AUTO VERSION INCREMENT

function readFile(filename, mode)
	file = io.open(filename, mode)

	content = nil

	if file ~= nil then
		content = file:read("*all")
	end

	file:close()

	return content
end

function writeFile(filename, mode, content)
	file = io.open(filename, mode)

	if file ~= nil then
		file:write(content)
	else
		print("[ERROR] Couldn't write to the file: "..filename)
	end

	file:close()
end


--
--
-- MAIN CODE
--
--

-- CHECKING THE ENVIRONMENT ARGUMENTS

if arg[1] == nil then
	print("[ERROR] You must specify version file name.")
	os.exit()
end

if arg[2] == nil then
	print("[ERROR] You must specify increment SYMBOL in the version file.")
	os.exit()
end

-- OPENING THE INPUT FILES (version file and git hash file)

version_file_content = readFile(arg[1], "r")

if version_file_content == nil then
	print("[ERROR] Couldn't open the version file: "..arg[1]..".")
	os.exit()
end

hash = readFile("githash", "r")

if hash == nil then
	print("[ERROR] Couldn't open the git hash file.")
	os.exit()
end

-- FINDING THE INCREMENT VERSION NUMBER TO UPDATE

magic_word = "#define "..arg[2]
location = version_file_content:find(magic_word)

if location == nil then
	print("[ERROR] Couldn't find the increment version keyword.")
	os.exit()
end

s = version_file_content:sub(location + magic_word:len() + 1)

version_number = nil
new_line_location = s:find("\n")

if new_line_location ~= nil then
	s = s:sub(0, new_line_location - 1) 
end

version_number = tonumber(s:match("%d+"))
		
if version_number ~= nil then
	version_number = version_number + 1
else
	print("[ERROR] Invalid increment version number.")
end

increment_version_line = version_file_content:sub(location)
increment_version_line = increment_version_line:sub(0, increment_version_line:find("\n"))

version_file_content = version_file_content:gsub(increment_version_line, "_@_DUMMY_@_\n")
version_file_content = version_file_content:gsub("_@_DUMMY_@_", magic_word.." "..version_number)

location = version_file_content:find("#define GIT_COMMIT_HASH_CODE")
git_hash_code_line = version_file_content:sub(location)
git_hash_code_line = git_hash_code_line:sub(0, git_hash_code_line:find("\n"))

version_file_content = version_file_content:gsub(git_hash_code_line, "_@_DUMMY_@_\n")
version_file_content = version_file_content:gsub("_@_DUMMY_@_", "#define GIT_COMMIT_HASH_CODE "..hash)

-- updating the version file with new increment version
writeFile(arg[1], "w", version_file_content)
