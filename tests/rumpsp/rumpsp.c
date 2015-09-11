#define MAXFDS 256
#define RUMPSP_EAGAIN -1

struct rumpsp_chan;

typedef void (*rumpsp_accept_fn)(struct rumpsp_chan *, void **token);
typedef void (*rumpsp_callback_fn)(struct rumpsp_chan *, void *token);

struct rumpsp_chan {
	void *token;

	int fd;
};

struct rumpsp_handlers {
	rumpsp_accept_fn accepted;
	rumpsp_callback_fn writable;
	rumpsp_callback_fn readable;
};

/*
static unsigned int maxidx;
static struct rumpsp_chan chanfds[MAXFDS];

static struct rumpsp_handlers handlers;

static unsigned int
getidx(struct rumpsp_chan *chan)
{

	return chan - chanfds;
}*/

static void
rumpsp_cleanup(void)
{

}

static int
rumpsp_init_server(const char *url, struct rumpsp_handlers hndlrs)
{
	return 0;
}

static int
rumpsp_read(struct rumpsp_chan *chan, void *data, size_t size, size_t *nbytes)
{

	return 0;
}

static int
rumpsp_write(struct rumpsp_chan *chan, void *data, size_t size, size_t *nbytes)
{
	return 0;
}

static void
rumpsp_close(struct rumpsp_chan *chan)
{

}

#define RUMPSP_EVENT_WRITABLE	0x1
#define RUMPSP_EVENT_READABLE	0x2

static int 
rumpsp_enable_events(struct rumpsp_chan *chan, int events)
{
	return 0;
}

static int 
rumpsp_disable_events(struct rumpsp_chan *chan, int events)
{
	return 0;
}

static int
rumpsp_dispatch(int timeout_ms)
{
	return 0;
}
