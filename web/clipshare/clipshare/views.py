from django.shortcuts import render


def index(request):

    if request.user.is_authenticated():
        return render(request, 'clipshare/index.html');
    else:
        return render(request, 'clipshare/landing.html');

def register(request):
    return render(request, 'clipshare/landing.html');

def login(request):
    return render(request, 'clipshare/landing.html');

def logout(request):
    return render(request, 'clipshare/landing.html');
