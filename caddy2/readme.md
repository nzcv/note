```shell
# Caddyfile
{
	auto_https off
}

:8080 {
	bind 0.0.0.0
	root * D:\\Android
	file_server browse
}
```

<!-- windows服务 -->
nssm.exe


.\nssm.exe install caddy2 I:\thirdshot\Caddy2\caddy.exe