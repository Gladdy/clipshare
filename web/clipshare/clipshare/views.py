import json

from django.shortcuts import render, redirect
from django.contrib.auth import logout, login, authenticate
from django.contrib import messages
from django.db import IntegrityError
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt

from clipshare.models import User, File


def index(request):
    if request.user.is_authenticated():
        return render(request, 'clipshare/index.html')
    else:
        return render(request, 'clipshare/landing.html')


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

        except ValueError as e:
            messages.add_message(request, messages.ERROR, e.args[0], extra_tags='login')
            messages.add_message(request, messages.INFO, request.POST.get('email'), extra_tags='email')

    return redirect('index')

@csrf_exempt
def apilogin_view(request):
    if request.method == 'POST':
        try:
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
            return JsonResponse({'authenticated': True}, status=202)

        except ValueError as e:
            return JsonResponse({'authenticated': False}, status=401)

    # Do not support GET'ing this endpoint
    return HttpResponse(status=405)


def logout_view(request):
    logout(request)
    return redirect('index')


@csrf_exempt
def upload_view(request):
    if request.method == 'POST':

        # Get an user object to use for storage
        if request.user.is_authenticated():
            user = request.user
        else:
            try:
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

            except ValueError:
                return HttpResponse(status=401)


        # Check whether there was an actual file specified
        if 'file' not in request.FILES:
            return HttpResponse("Uploading but not providing a file??", status=400)

        # Attempt saving it
        try:
            f = File.manager.store(user, request.FILES['file'])
            return JsonResponse({'url': f.url }, status=201)

        except IOError as e:
            return HttpResponse(str(e), status=400)


    return HttpResponse("This is just the get")

def terms(request):
    return render(request, 'clipshare/terms.html')
