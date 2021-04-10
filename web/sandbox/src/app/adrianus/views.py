from django.shortcuts import render


def home(request):
    env = {
    	'is_logged_in': False,
    	'username': None,
    	'userid': None,
    	'name': None
    }
    return render(request, 'home.html', env)


def guest_login(request):
	env = {
		'is_logged_in': True,
		'usermame': 'Guest215486',
		'userid': -1,
		'name': 'Guest215486'
	}
	return render(request, 'guest-login.html', env)