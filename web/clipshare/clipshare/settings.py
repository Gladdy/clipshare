import os
import settings_local as LOCAL
from boto3.session import Session


BASE_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# SECURITY WARNING: keep the secret key used in production secret!
SECRET_KEY = LOCAL.SECRET_KEY
DEBUG = True
ALLOWED_HOSTS = []
AUTH_USER_MODEL = 'emailauth.User'
AUTHENTICATION_BACKENDS = ['emailauth.backends.EmailAuthBackend', ]



# Application definition
INSTALLED_APPS = (
    'django.contrib.admin',
    'django.contrib.auth',
    'django.contrib.contenttypes',
    'django.contrib.sessions',
    'django.contrib.messages',
    'django.contrib.staticfiles',
    'emailauth',
    'api',
    'clipshare',
)

MIDDLEWARE_CLASSES = (
    'django.contrib.sessions.middleware.SessionMiddleware',
    'django.middleware.common.CommonMiddleware',
    'django.middleware.csrf.CsrfViewMiddleware',
    'django.contrib.auth.middleware.AuthenticationMiddleware',
    'django.contrib.auth.middleware.SessionAuthenticationMiddleware',
    'django.contrib.messages.middleware.MessageMiddleware',
    'django.middleware.clickjacking.XFrameOptionsMiddleware',
    'django.middleware.security.SecurityMiddleware',
)

ROOT_URLCONF = 'clipshare.urls'
WSGI_APPLICATION = 'clipshare.wsgi.application'


TEMPLATES = [
    {
        'BACKEND': 'django.template.backends.django.DjangoTemplates',
        'DIRS': [os.path.join(BASE_DIR, 'templates/')],
        'APP_DIRS': True,
        'OPTIONS': {
            'context_processors': [
                'django.template.context_processors.debug',
                'django.template.context_processors.request',
                'django.contrib.auth.context_processors.auth',
                'django.contrib.messages.context_processors.messages',
            ],
        },
    },
]

# Data sources
DATABASES = LOCAL.DATABASES

# S3
awssession = Session(aws_access_key_id=LOCAL.AWS_ACCESS_KEY_ID
                     ,  aws_secret_access_key=LOCAL.AWS_SECRET_ACCESS_KEY
                     ,  region_name='eu-central-1')
s3 = awssession.resource('s3')
AWS_STORAGE_BUCKET_NAME = 'clipshare-storage'
AWS_S3_ROOT = 'https://s3.amazonaws.com/'


# Internationalization
LANGUAGE_CODE = 'en-us'
TIME_ZONE = 'UTC'
USE_I18N = True
USE_L10N = True
USE_TZ = True


# Static files (CSS, JavaScript, Images)
STATIC_URL = '/static/'
STATIC_DIR = 'static'
STORAGE_DIR = '/srv/clipshare/'
SITE_URL = 'http://localhost:8000'

STATICFILES_DIRS = (
    os.path.join(BASE_DIR, STATIC_DIR),
    STORAGE_DIR,
)