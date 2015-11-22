from django.shortcuts import render
from django.contrib.auth.decorators import user_passes_test
from django.views.decorators.http import require_http_methods
from django.shortcuts import redirect

from clipshare.settings import s3, AWS_STORAGE_BUCKET_NAME
from api.models import File
from api.managers import random_id
from emailauth.models import User

def index(request):

    if request.user.is_superuser:
        return render(request, 'clipshare/superuser.html', {
            'files': File.objects.all()[:100],
            'randomstring': random_id(length=16),
        })

    elif request.user.is_authenticated():
        return render(request, 'clipshare/index.html', {
            'files': request.user.file_set.all(),
        })
    else:
        return render(request, 'clipshare/landing.html')


def terms(request):
    return render(request, 'clipshare/terms.html')


@require_http_methods(["POST"])
@user_passes_test(lambda u: u.is_superuser)
def wipe_view(request):
    try:
        if ('source' not in request.POST) \
                or ('target' not in request.POST) \
                or request.POST['source'] != request.POST['target']:
            raise ValueError("Invalid copy of strings")

        bucket = s3.Bucket(AWS_STORAGE_BUCKET_NAME)

        for key in bucket.objects.all():
            key.delete()

        for file in File.objects.all():
            file.delete()

    except Exception as e:
        print(e)
        pass

    return redirect('index')