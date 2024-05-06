local frandom = io.open("/dev/urandom", "rb")
local d = frandom:read(4)
math.randomseed(d:byte(1) + (d:byte(2) * 256) + (d:byte(3) * 65536) + (d:byte(4) * 4294967296))

request = function()
    headers = {}
    headers["Content-Type"] = "application/json"
	headers["Authorization"] = "Basic c2FtcGxlX2xvZ2luXzE6MTIzNDU2Nzg="
    body = ''
    return wrk.format("GET", "/accommodation?id=".. tostring(math.random(1, 3)), headers, body)
end