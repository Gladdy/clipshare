from django.shortcuts import redirect
from django.contrib.auth import logout, login, authenticate
from django.contrib import messages
from django.db import IntegrityError

from emailauth.models import User


def attempt_login(request):
    if set(('email', 'password')) > set(request.POST) \
            or len(request.POST['email']) < 5 \
            or request.POST['password'] == '':
        raise ValueError("Please fill in all fields")

    password = request.POST['password']
    email = request.POST['email']

    try:
        user = authenticate(email=email, password=password)
    except User.DoesNotExist:
        raise ValueError("Invalid login credentials")

    login(request, user)


def register_view(request):
    if request.method == 'POST':
        try:
            if set(('email', 'password1', 'password2')) > set(request.POST) \
                    or len(request.POST['email']) < 5 \
                    or request.POST['password1'] == '' \
                    or request.POST['password2'] == '':
                raise ValueError("Please fill in all fields")

            if not 'terms' in request.POST:
                raise ValueError("Please accept the terms and conditions")

            password1 = request.POST['password1']
            password2 = request.POST['password2']
            email = request.POST['email']

            if password1 != password2:
                raise ValueError("Please make sure the passwords match")

            try:
                User.objects.create_user(email, password1)
            except IntegrityError:
                raise ValueError("This email address is already in use")

            user = authenticate(email=email, password=password1)
            login(request, user)

        except ValueError as e:
            messages.add_message(request, messages.ERROR, e.args[0], extra_tags='register')
            messages.add_message(request, messages.INFO, request.POST.get('email'), extra_tags='email')
            return redirect('/#signup')

    return redirect('index')


def login_view(request):
    if request.method == 'POST':
        try:
            attempt_login(request)
        except ValueError as e:
            messages.add_message(request, messages.ERROR, e.args[0], extra_tags='login')
            messages.add_message(request, messages.INFO, request.POST.get('email'), extra_tags='email')

    return redirect('index')


def logout_view(request):
    logout(request)
    return redirect('index')