from django.shortcuts import render

from clipshare.settings import s3, AWS_STORAGE_BUCKET_NAME


def index(request):

    if request.user.is_superuser:
        bucket = s3.Bucket(AWS_STORAGE_BUCKET_NAME)


        return render(request, 'clipshare/superuser.html', {
            'bucketobjects': bucket.objects.all(),
        })

    elif request.user.is_authenticated:
        return render(request, 'clipshare/index.html')
    else:
        return render(request, 'clipshare/landing.html')


def terms(request):
    return render(request, 'clipshare/terms.html')
