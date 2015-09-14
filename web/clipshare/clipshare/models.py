from django.contrib.auth.models import AbstractBaseUser, PermissionsMixin
from django.db import models
from django.db.models.manager import Manager
from clipshare.managers import UserManager, FileManager


class User(AbstractBaseUser, PermissionsMixin):
    """
    Custom user class.
    """
    email = models.EmailField('email address', unique=True, db_index=True)
    is_staff = models.BooleanField(default=False)
    is_active = models.BooleanField(default=True)
    date_joined = models.DateTimeField(auto_now_add=True)

    USERNAME_FIELD = 'email'
    objects = UserManager()

    def __str__(self):
        return self.email

    def get_short_name(self):
        return self.email

    def get_full_name(self):
        return self.email


class UserProfile(models.Model):
    user = models.OneToOneField(User, primary_key=True)



class File(models.Model):
    id = models.CharField(max_length=16, primary_key=True)
    user = models.ForeignKey(User)

    filename = models.TextField()
    filesize = models.IntegerField()
    mimetype = models.CharField(max_length=64)
    location = models.FilePathField()
    url = models.URLField()

    objects = Manager()
    manager = FileManager()

