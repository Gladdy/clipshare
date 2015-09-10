from django.shortcuts import render, redirect
from django.contrib.auth import logout, login, authenticate


from clipshare.forms import AuthenticationForm, RegistrationForm

def index(request):
    if request.user.is_authenticated():
        return render(request, 'clipshare/index.html')
    else:
        return render(request, 'clipshare/landing.html', {
            'registrationform': RegistrationForm(),
            'authenticationform': AuthenticationForm(),
        })


def register_view(request):
    """
    User registration view.
    """
    if request.method == 'POST':
        form = RegistrationForm(data=request.POST)
        if form.is_valid():
            user = form.save()
            user = authenticate(email=request.POST['email'], password=request.POST['password1'])
            login(request, user)
            return redirect('index')
    else:
        form = RegistrationForm()

    return render(request, 'clipshare/landing.html', {
        'registrationform': form,
        'authenticationform':AuthenticationForm,
    })


def login_view(request):
    """
    Log in view
    """
    if request.method == 'POST':
        form = AuthenticationForm(data=request.POST)
        if form.is_valid():
            user = authenticate(email=request.POST['email'], password=request.POST['password'])
            if user is not None:
                if user.is_active:
                    login(request, user)
                    return redirect('index')
    else:
        form = AuthenticationForm()

    return render(request, 'clipshare/landing.html', {
        'registrationform': RegistrationForm,
        'authenticationform':form,
    })


def logout_view(request):
    logout(request)
    return redirect('index')