搭建服务器的时候,遇到了反向代理和文件服务的问题

https://caddy.community/t/file-server-reverse-proxy/10664/3

```shell
emr.cure.org {

	handle_path /metabase* {
		reverse_proxy localhost:3000
	}

	handle {
		root * /usr/share/caddy
		file_server
	}
}
```