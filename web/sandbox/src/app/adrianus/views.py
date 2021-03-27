from django.shortcuts import render


def home(request):
    env = {}
    return render(request, 'home.html', env)