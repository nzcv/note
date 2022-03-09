since we're not pumping this continually and we only draw when we need to, we can just do the
full initialisation and teardown every time. This means we don't have to pay attention to
whether something else needs to create a context on the window - we just do it
opportunistically when we can hold the draw lock.
This only takes about 30ms anyway, so it's still technically realtime, right!?

fetch everything dynamically. We can't link against libEGL otherwise it messes with the hooking
  in the main library. Just declare local function pointers with the real names