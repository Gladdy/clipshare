from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods
from django.shortcuts import redirect

from emailauth.views import attempt_login
from api.models import File


@csrf_exempt
@require_http_methods(["POST"])
def upload_view(request):
    try:
        # Get an user object to use for storage
        if request.user.is_authenticated():
            user = request.user
        else:
            attempt_login(request)

        # Check whether there was an actual file specified
        if 'file' not in request.FILES:
            raise ValueError("File not found in request")

        # Attempt storing it
        f = File.manager.store(user, request.FILES['file'])

        if 'indexredirect' in request.POST:
            return redirect('index')

        # Return the url of the resource
        return JsonResponse({'url': f.url}, status=201)

    except (IOError, ValueError) as e:
        return JsonResponse({'error': str(e)}, status=400)


@csrf_exempt
@require_http_methods(["POST"])
def login_view(request):
    try:
        attempt_login(request)
        return JsonResponse({'authenticated': True}, status=202)

    except ValueError as e:
        return JsonResponse({'authenticated': False}, status=401)