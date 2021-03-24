from django.shortcuts import render


def check2(request):
    env = {}
    return render(request, 'check2.html', env)