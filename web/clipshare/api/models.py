from django.contrib.auth.models import AbstractBaseUser, PermissionsMixin
from django.db import models
from django.db.models.manager import Manager

from api.managers import FileManager
from emailauth.models import User


class File(models.Model):
    id = models.CharField(max_length=16, primary_key=True)
    user = models.ForeignKey(User)

    filename = models.TextField()
    filesize = models.IntegerField()
    mimetype = models.CharField(max_length=64)
    url = models.URLField()

    objects = Manager()
    manager = FileManager()

